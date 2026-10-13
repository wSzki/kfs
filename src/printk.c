/*
 * src/printk.c -- the kernel's eyes, ears and mouth.
 * ============================================================================
 *
 * ROLE IN THE BOOT CHAIN
 * ----------------------------------------------------------------------------
 * Before this file does its job, the kernel is INVISIBLE. A kernel that
 * panics silently tells you nothing: the QEMU window just shows the last
 * boot screen and you are left staring. This file is the FIRST thing that
 * makes the kernel OBSERVABLE: from the moment kmain() runs, every line of
 * the program (GDT summary, stack dump, shell output) is rendered here, in
 * two independent places:
 *
 *   - the VGA text screen (memory-mapped at physical address 0xB8000)
 *     -> what you see in the QEMU window
 *   - the COM1 serial port (I/O port 0x3F8, a UART 16550)
 *     -> what `qemu-system-i386 -kernel kernel.elf -display none
 *        -serial stdio` pipes into your host terminal
 *
 * Having BOTH is the whole point: the screen needs a display, the serial
 * port needs none. Headless CI, remote debugging, "where did it die?" all
 * run through the serial mirror, one byte at a time, in perfect lockstep
 * with the screen.
 *
 * Where this file sits, read the boot-flow diagram at the TOP of
 * include/kernel.h: BIOS -> GRUB -> boot.asm -> kmain() -> printk().
 * Everything else in the kernel (gdt_install, stack_dump, shell_run) is
 * built on the assumption that "I can say what I am doing".
 *
 * CONCEPTS THIS FILE INTRODUCES
 * ----------------------------------------------------------------------------
 *   1. Memory-mapped I/O: the screen is not a device you "talk to" with a
 *      call, it is a byte array at a physical address. Writing one uint16_t
 *      to 0xB8000 + offset changes the pixel the VGA card is showing.
 *   2. The volatile qualifier and why device memory MUST be volatile.
 *   3. Port I/O (IN/OUT instructions): the UART and the CRT cursor live on
 *      a second address space, reached with the outb/inb helpers declared
 *      in include/kernel.h (section 2 of that header).
 *   4. va_list varargs: how a function can take "as many arguments as you
 *      like and of different types" (stdarg.h).
 *   5. Hand-rolled number-to-string: decimal and hex, including the
 *      INT_MIN trap and the unsigned-negate trick.
 *
 * FURTHER READING (read in this order, roughly 45 minutes)
 * ----------------------------------------------------------------------------
 *   - OSDev "Bare Bones"              https://wiki.osdev.org/Bare_Bones
 *       (the minimal kernel skeleton; our project is this plus a GDT)
 *   - OSDev "VGA Hardware"            https://wiki.osdev.org/VGA_Hardware
 *       (the 0xB8000 framebuffer, 0x3D4/0x3D5 cursor ports, palette)
 *   - OSDev "Serial Ports"            https://wiki.osdev.org/Serial_Ports
 *       (port map, 16550 architecture, DLAB/divisor/baud-rate math)
 *   - OSDev "Printing To Serial Port" https://wiki.osdev.org/Printing_To_Serial_Port
 *       (the transmitter-holding-empty wait loop we implement below)
 *   - Intel SDM Vol. 3, ch. 23.6     (16550 datasheet summary)
 * ============================================================================
 */

#include "../include/kernel.h"   /* outb/inb, uint*_t, the public API */

/* ===========================================================================
 * PART 1: THE VGA TEXT CONSOLE (0xB8000)
 * ===========================================================================
 *
 * HOW A TEXT MODE SCREEN ACTUALLY WORKS
 * ----------------------------------------------------------------------------
 * In 80x25 text mode (the default after the BIOS boots), the VGA card does
 * NOT store pixels. It stores a table of 80*25 = 2000 CELLS, and each cell
 * is TWO BYTES long:
 *
 *   byte 0 (low byte)  = the ASCII character to show (glyph from CGA font)
 *   byte 1 (high byte) = the attribute byte: which colors to draw it with
 *
 * The card reads this table from physical memory starting at 0xB8000 and
 * renders whatever is there. It does not "know" that a CPU wrote those
 * bytes. So the entire screen can be manipulated by plain C stores:
 *
 *   0xB8000:  [ 'K' ][ 07 ] [ 'F' ][ 07 ] [ 'S' ][ 07 ] ...  <- row 0
 *              \____/      \____/      \____/
 *              cell 0       cell 1       cell 2
 *   ...         80 cells per row, 25 rows, 2000 cells total ...
 *   0xB9FA0:   <- one past the last byte (0xB8000 + 2000*2 = 0xB9FA0)
 *
 * A cell is addressed as vga_mem[row*80 + col]. Little-endian means the
 * char byte comes first: cell value 0x0741 is stored in memory as 41 07
 * ('A' followed by attribute 0x07).
 *
 * THE ATTRIBUTE BYTE, BIT BY BIT
 * ----------------------------------------------------------------------------
 *   bit 7..4   (high nibble)  = background color index
 *   bit 3..0   (low  nibble)  = foreground color index
 *
 * i.e.  attribute = (bg << 4) | fg
 *
 * The VGA palette has 16 colors, numbered 0..15:
 *   0 black    1 blue      2 green    3 cyan
 *   4 red      5 magenta   6 brown    7 light gray
 *   8 dark gray 9 light blue 10 light green 11 light cyan
 *   12 light red 13 pink  14 yellow   15 white
 *
 * We use 0x07 everywhere:
 *   0x07 = 0000_0111 (binary)
 *          ^^^^ ^^^^
 *          |    +-- foreground = 7 = light gray
 *          +------- background = 0 = black
 *
 * So (bg << 4) | fg with bg = 0, fg = 7 gives 0*16 + 7 = 0x07.
 *
 * WHY THE POINTER MUST BE volatile -- read this, it is the most common
 * "why does it work with -O2 but not with -O0" bug in kernel code:
 * ----------------------------------------------------------------------------
 * The VGA framebuffer looks like ordinary RAM, but it is NOT RAM as far as
 * the program is concerned: it is DEVICE MEMORY. The VGA card sits on the
 * other side of those addresses and reads what we write. From the
 * compiler's point of view, a plain (non-volatile) store through a pointer
 * is "just memory": if the compiler sees that the stored value is never
 * read back later, it is ALLOWED to optimize the store away entirely
 * (dead-store elimination). It can also:
 *   - cache a cell's old value in a register and skip re-reading it,
 *   - merge two consecutive identical writes into one,
 *   - hoist/reorder the whole scroll loop in ways the hardware does not
 *     expect.
 * None of that is a bug in the compiler: it is doing EXACTLY what the
 * C language says to do. The qualifier `volatile` changes the contract:
 * "this object can change in ways I cannot see, and every access to it
 * has a real effect the program depends on". The compiler then emits a
 * genuine store for EVERY write and a genuine load for EVERY read, every
 * time. This is why every textbook kernel writes
 *
 *     static volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
 *
 * (The serial inb/outb helpers in include/kernel.h are written the same
 * way: `__asm__ volatile` for the same reason -- a device read/write must
 * never be deleted or resequenced.)
 *
 * WHY WE COPY THE SCROLL BY HAND (no memcpy!)
 * ----------------------------------------------------------------------------
 * Two reasons:
 *   1. We are FREESTANDING. With -ffreestanding -nostdlib, the libc that
 *      defines memcpy() (inside <string.h>) simply does not exist on the
 *      target. There is no malloc, no printf, no memcpy.
 *   2. Even if memcpy() did exist, calling it on a VOLATILE region would
 *      be WRONG: its signature takes `void *` (a non-volatile pointer),
 *      so the stores it emits internally are NOT guaranteed to be
 *      volatile-qualified. The hardware would not see them. A hand-rolled
 *      loop that walks the volatile pointer directly is the only correct
 *      way to move device memory.
 * =========================================================================== */

/* The framebuffer base. 0xB8000 is hardcoded into the ISA/AGP bus design
 * of every PC: the text-mode framebuffer physically lives there. GRUB
 * leaves the card in 80x25 mode, and nothing between GRUB and us changes
 * it, so we can rely on it. */
#define VGA_BASE_ADDR   0xB8000u
#define VGA_COLS        80
#define VGA_ROWS        25
#define VGA_TOTAL_CELLS (VGA_COLS * VGA_ROWS)   /* 2000 */

/* One cell = character | attribute << 8.  0x07 = black bg, light gray fg
 * (the bit math is at the top of this file). */
#define VGA_ATTR    0x07
#define VGA_CELL(c) ((uint16_t)(c) | ((uint16_t)VGA_ATTR << 8))
/* A "blank" cell: a space character with the attribute, used to clear
 * the bottom row after a scroll. */
#define VGA_BLANK   VGA_CELL(' ')

/* The volatile pointer, the ONLY correct way to reach the screen. */
static volatile uint16_t *vga_mem = (volatile uint16_t *)VGA_BASE_ADDR;

/* Where the "current character" is. Plain C variables, NOT hardware: the
 * hardware only shows the cursor where the CRTC registers say (below), so
 * we update row/col, then push the position to the card. */
static int cursor_row = 0;
static int cursor_col = 0;

/* The one-time serial init flag: set once in printk() before the first
 * byte goes out. 0 until then. */
static int serial_ready = 0;

/* ===========================================================================
 * PART 2: THE UART (COM1) AT 0x3F8
 * ===========================================================================
 *
 * WHAT IS A UART?
 * ----------------------------------------------------------------------------
 * COM1 is a 16550 UART (Universal Asynchronous Receiver-Transmitter): a
 * tiny chip that turns parallel bytes from the CPU into serial bits on a
 * wire (and back). QEMU emulates one; on real hardware it is a physical
 * chip behind a DB9 connector. The whole console mirror goes through it.
 *
 * THE REGISTER MAP (COM1 = base 0x3F8, the 16550's 8 registers):
 *
 *   offset  port   register              meaning
 *   ------  -----   -------------------   ----------------------------
 *   +0      0x3F8  THR / RBR             TX hold register (write: byte to
 *                    DIV-LSB            send) / RX buffer (read: byte in)
 *   +1      0x3F9  IER / ISR             interrupt enable / status
 *                    DIV-MSB
 *   +2      0x3F8+2 FCR / IFC            FIFO control / ID flag
 *   +3      0x3FB  LCR                   line control (word size, DLAB)
 *   +4      0x3FC  MCR                   modem control
 *   +5      0x3FD  LSR                   line status (the "ready?" flags)
 *   +6      0x3FE  MSR                   modem status
 *   +7      0x3FF  SCR                   scratch (junk register)
 *
 * THE DLAB TRICK -- why two registers share one address:
 * ----------------------------------------------------------------------------
 * When LCR bit 7 (DLAB) is 1, the ports +0 and +1 are re-routed: they no
 * longer hold THR/IER but instead hold the low and high byte of the
 * BAUD-RATE DIVISOR. When DLAB is 0, the ports behave normally. This is
 * how the 16550 squeezes 10 logical registers into 8 physical ports.
 * Every UART initialization sequence is: "DLAB on, write divisor, DLAB
 * off, configure word size".
 *
 * BAUD-RATE MATH FOR 115200
 * ----------------------------------------------------------------------------
 * The UART's clock input is 1.8432 MHz (1843200 Hz). The 16550 divides it
 * by 16 AND by the (programmable) DIVISOR:
 *
 *   baud = 1843200 / 16 / DIVISOR
 *
 * For 115200 baud:
 *   DIVISOR = 1843200 / (16 * 115200)
 *           = 1843200 / 1843200
 *           = 1
 *
 * So 115200 is the "natural" rate of a 1.8432 MHz clock -- divisor is
 * exactly 1 (9600 would need 193; 19200 would need 96). QEMU emulates
 * this clock faithfully, so divisor 1 gives a perfect 115200.
 *
 * THE TRANSMIT WAIT (why the loop is there at all)
 * ----------------------------------------------------------------------------
 * After outb(0x3F8, c), the byte does NOT leave the chip instantly: it
 * sits in the THR, then moves to the 8-BIT SHIFT REGISTER, and only
 * after THAT is the line free. LSR (port 0x3FD) reports:
 *
 *   bit 5  THRE: Transmitter Holding Register Empty -- the THR is free,
 *          the previous byte has been handed to the shift register.
 *   bit 6  TEMT: Transmitter EMPTy -- the SHIFT REGISTER is also empty,
 *          the line is truly idle.
 *
 * We wait for THRE (bit 5) before writing the NEXT byte: that guarantees
 * byte ordering under polling. At 115200 a byte takes ~87 microseconds,
 * so at this speed the loop almost never spins more than once or twice.
 * (Interrupt-driven output would use an IRQ, but our kernel keeps
 * interrupts off this iteration -- the shell polls the keyboard the same
 * way. The wait loop is the polling version of "wait for the interrupt".)
 *
 * REFERENCE: https://wiki.osdev.org/Serial_Ports and
 * https://wiki.osdev.org/Printing_To_Serial_Port
 * =========================================================================== */

/* The 8 UART ports (COM1 base 0x3F8). */
#define COM1_THR   0x3F8u   /* TX hold / RX buffer / divisor-low  (offset 0) */
#define COM1_IER   0x3F9u   /* IER / ISR / divisor-high         (offset 1) */
#define COM1_FCR   0x3FAu   /* FIFO control / ID flags          (offset 2) */
#define COM1_LCR   0x3FBu   /* Line control                     (offset 3) */
#define COM1_MCR   0x3FCu   /* Modem control                    (offset 4) */
#define COM1_LSR   0x3FDu   /* Line status                      (offset 5) */
#define COM1_MSR   0x3FEu   /* Modem status                     (offset 6) */
#define COM1_SCR   0x3FFu   /* Scratch register                 (offset 7) */

/* LSR bit 5: THRE (transmitter holding register empty) -- the one we
 * wait on before each byte. (Bit 6, TEMT, would be the stricter
 * "line fully idle" check; THRE is the standard polled-output test.) */
#define COM1_LSR_THRE   (1u << 5)

/* LCR bits: 0x03 = 8 data bits, no parity, 1 stop bit ("8N1").
 * Bit 7 (0x80) is DLAB, used transiently during init only. */
#define COM1_LCR_8N1    (1u << 1)   /* 0x02: 8 data bits          */
#define COM1_LCR_NOPAR  (0u)       /* 0x00: no parity            */
#define COM1_LCR_1STOP  (0u)       /* 0x00: 1 stop bit           */
#define COM1_LCR_DLAB   (1u << 7)  /* 0x80: divisor-latch access */

/* FCR bits: 0x07 = enable FIFOs + clear TX FIFO + clear RX FIFO. */
#define COM1_FCR_ENABLE (1u << 0)
#define COM1_FCR_CLR_TX (1u << 1)
#define COM1_FCR_CLR_RX (1u << 2)

/* MCR bits: DTR (bit 0) + RTS (bit 1) = "bring the line up". QEMU's
 * pty-backed serial behaves like a real terminal ONLY when these are
 * asserted -- without them the output can be silently dropped. */
#define COM1_MCR_DTR    (1u << 0)
#define COM1_MCR_RTS    (1u << 1)

/* One-time UART setup. Called lazily from printk(), guarded by the
 * `serial_ready` static flag (see its use below). Every field explained
 * in the register map above. */
static void serial_init(void)
{
    /* Step 0: mask all interrupts. We POLL the LSR loop in serial_putc,
     * never use IRQ4 -- but leaving IER=0 here is the belt-and-suspenders
     * move: GRUB/BIOS may have left the chip in any state. */
    outb(COM1_IER, 0x00);

    /* Step 1: unlock the divisor latches (DLAB = LCR bit 7). While DLAB
     * is set, port 0x3F8 becomes DIV-LSB and 0x3F9 becomes DIV-MSB. */
    outb(COM1_LCR, COM1_LCR_DLAB);

    /* Step 2: write the divisor for 115200 baud.
     *   DIVISOR = 1843200 / (16 * 115200) = 1
     * Low byte -> 0x3F8, high byte -> 0x3F9. */
    outb(COM1_THR, 1);    /* DIV-LSB = 1    */
    outb(COM1_IER, 0);    /* DIV-MSB = 0    */

    /* Step 3: 8N1 word format, DLAB back off. From here on, 0x3F8 is THR
     * again and we can actually send bytes. */
    outb(COM1_LCR, COM1_LCR_8N1 | COM1_LCR_NOPAR | COM1_LCR_1STOP);  /* 0x03 */

    /* Step 4: enable + clear the TX/RX FIFOs. The 16450 (a real chip that
     * still exists in old machines) had no FIFOs at all; on a 16450 this
     * write is a no-op, on a 16550 it is required for correct batching. */
    outb(COM1_FCR, COM1_FCR_ENABLE | COM1_FCR_CLR_TX | COM1_FCR_CLR_RX); /* 0x07 */

    /* Step 5: assert DTR + RTS so the line is "up" (see the note on MCR
     * above). */
    outb(COM1_MCR, COM1_MCR_DTR | COM1_MCR_RTS);  /* 0x03 */
}

/* Send ONE byte to COM1.
 *
 * The loop waits for THRE (LSR bit 5): the holding register is free, i.e.
 * the previous byte has been handed off to the shift register. THEN the
 * new byte is written to the THR. At 115200 the wait is ~87us and the
 * loop spins at most a couple of times; it is the correct "polled,
 * no-IRQ" discipline and it is what OSDev's "Printing To Serial Port"
 * article shows. */
static void serial_putc(char c)
{
    while (!(inb(COM1_LSR) & COM1_LSR_THRE))
        ;                       /* spin until the holding register is free */
    outb(COM1_THR, (uint8_t)c);
}

/* ===========================================================================
 * PART 3: THE HARDWARE CURSOR (ports 0x3D4 / 0x3D5)
 * ===========================================================================
 *
 * The blinking block on the screen is not a character; it is a 2-D
 * rectangle the VGA card draws OVER the current cell. Two groups of CRTC
 * (CRT controller) registers control it:
 *
 *   register 0x0A: cursor START scanline  (shape: where the block begins)
 *   register 0x0B: cursor END   scanline  (shape: where the block stops)
 *   register 0x0E: cursor LOCATION high byte (WHERE the block is drawn)
 *   register 0x0F: cursor LOCATION low  byte
 *
 * The LOCATION registers hold a single 16-bit number: the CELL index
 * (row * 80 + col) of the cell to draw the cursor over. Because it is
 * 16 bits and each CRTC register is 8 bits wide, it is split across the
 * high (0x0E) and low (0x0F) halves -- exactly like splitting a uint16_t
 * into two uint8_t's.
 *
 * You reach any CRTC register through the index/data port PAIR
 * 0x3D4/0x3D5: first `outb(0x3D4, index)` selects which register, THEN
 * `outb(0x3D5, value)` reads/writes its value. This index/data pattern
 * is how x86 maps hundreds of CRTC registers onto two I/O ports.
 *
 * We only move the cursor (0x0E/0x0F) and leave its SHAPE registers
 * (0x0A/0x0B) at whatever the BIOS set them to, so the cursor keeps the
 * classic BIOS look. (Set start=14, end=15 on VGA for a thin underline
 * cursor; start bit 6 enables blink -- not needed here.) */
static void vga_set_cursor(void)
{
    /* The cursor position in CELLS, not scanlines: the CRTC's location
     * register is the 16-bit cell index (row*80 + col). */
    uint16_t pos = (uint16_t)(cursor_row * VGA_COLS + cursor_col);

    /* Low byte first (index 0x0F), then high byte (index 0x0E). The
     * order between the two does not matter for the location, but BOTH
     * must be written for the cursor to jump atomically to `pos`. */
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));         /* location low byte  */
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));  /* location high byte */
}

/* ===========================================================================
 * PART 4: SCROLLING (move 24 rows up, clear the last row)
 * ===========================================================================
 *
 * The screen has no hardware scroll; "scroll" is 24 rows of memory being
 * moved up by one row, the last row filled with spaces, and the cursor
 * snapped back to row 24 col 0. We do it by hand (see the "why no
 * memcpy" note at the top of this file).
 *
 * The copy loop runs top-to-bottom (dest row 0..23 = src row 1..24).
 * That is the SAFE direction: every cell we read (row r+1) is BELOW the
 * cell we write (row r), so the read happens BEFORE the write to that
 * same row. A bottom-to-top loop would overwrite the source row before
 * it was read, and the whole screen would come out full of the old row
 * 23. */
static void vga_scroll(void)
{
    /* Move rows 1..24 up to rows 0..23: 24 rows * 80 cells = 1920 cells,
     * each source cell is 80 positions (one row) ahead of its dest. */
    for (int i = 0; i < (VGA_ROWS - 1) * VGA_COLS; i++)
        vga_mem[i] = vga_mem[i + VGA_COLS];

    /* Blank the (now-duplicate) last row: 80 spaces with the attribute. */
    for (int i = VGA_ROWS * VGA_COLS - VGA_COLS; i < VGA_TOTAL_CELLS; i++)
        vga_mem[i] = VGA_BLANK;

    /* We are now on the LAST row, col 0. The cursor stays visible; the
     * next character will land right here. */
    cursor_row = VGA_ROWS - 1;
    cursor_col = 0;
}

/* ===========================================================================
 * PART 5: vga_putc / console_putc -- the two funnels every byte goes through
 * ===========================================================================
 *
 * vga_putc(c)      : write ONE character to the screen (and advance the
 *                    cursor / scroll if needed).
 * console_putc(c)   : vga_putc(c) + serial_putc(c) -- the "one byte, two
 *                    destinations" funnel. EVERY formatted byte in this
 *                    file passes through this function exactly once, so
 *                    the screen and the serial mirror can never disagree. */

static void vga_putc(char c)
{
    /* Backspace: step the cursor back one cell (never before column 0)
     * and BLANK the cell we leave, so the erased character visibly
     * disappears. This is what makes the shell's line editing work:
     * the shell just prints '\b' and vga_putc does the erase.
     * (The serial mirror forwards the raw 0x08 byte; a real terminal
     * interprets it the same way.) */
    if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            vga_mem[cursor_row * VGA_COLS + cursor_col] = VGA_BLANK;
        }
        vga_set_cursor();
        return;
    }

    /* Carriage return: jump to column 0 WITHOUT advancing the row (this
     * is what the '\r' character means; '\n' is the one that advances
     * the row). Neither writes anything to the screen. */
    if (c == '\r') {
        cursor_col = 0;
        vga_set_cursor();
        return;
    }

    /* Newline: wrap to col 0, advance row, scroll if we walked off. The
     * character itself is not stored; it is the newline marker. */
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= VGA_ROWS)
            vga_scroll();
        vga_set_cursor();
        return;
    }

    /* Store the cell: character in the low byte, attribute in the high
     * byte. Little-endian: the card reads the low byte as the glyph and
     * the high byte as the color pair. */
    vga_mem[cursor_row * VGA_COLS + cursor_col] = VGA_CELL(c);

    /* Advance the cursor; wrap to the next row on col overflow. */
    cursor_col++;
    if (cursor_col >= VGA_COLS) {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= VGA_ROWS)
            vga_scroll();
    }

    /* Push the hardware cursor to its new cell. Doing this per-character
     * is a little work (four port writes per char) but it is the "right"
     * thing for a teaching kernel: the user always sees where we are. In
     * a production kernel you would batch the cursor update (do it only
     * at scroll / row-wrap / end of line) and save the port writes. */
    vga_set_cursor();
}

static void console_putc(char c)
{
    vga_putc(c);
    serial_putc(c);
}

/* ===========================================================================
 * PART 6: HAND-ROLLED NUMBER TO STRING (decimal + hex)
 * ===========================================================================
 *
 * WHY THE REVERSE-BUFFER TRICK (and why it is at all)
 * ----------------------------------------------------------------------------
 * We compute the LEAST-significant digit of a number FIRST:
 *
 *   1337 % 10 = 7   (the last digit)
 *   1337 / 10 = 133
 *   133  % 10 = 3
 *   133  / 10 = 13
 *   13   % 10 = 3
 *   13   / 10 = 1
 *   1    % 10 = 1
 *   1    / 10 = 0    (done)
 *
 * But we must PRINT "1337", not "7331". Division peels digits off the
 * BACK of the number, so we collect them into a small buffer in REVERSE
 * order, then walk the buffer FORWARD. The buffer must hold the max case:
 * a uint32_t decimal number has at most 10 digits (4294967295), and a
 * hex number at most 8 nibbles. The helpers below write into small
 * fixed-size buffers and return the count of characters written.
 *
 * THE INT_MIN EDGE CASE (and why the trick below is well-defined)
 * ----------------------------------------------------------------------------
 * int32_t ranges from -2147483648 to +2147483647: 256 negative numbers
 * for every 255 positive ones (zero takes a slot). So the magnitude of
 * INT_MIN is 2147483648, which DOES NOT FIT in an int32_t -- the classic
 * trap: `v = -v` on v = INT_MIN overflows, which is UNDEFINED BEHAVIOR
 * in standard C (on x86 it "works" because two's-complement negation
 * wraps, but relying on that is a code smell and a portability hazard).
 *
 * The clean fix: do the negation in the UNSIGNED domain, where the
 * standard guarantees modular (2^32) wraparound for overflow:
 *
 *     uint32_t v = 0u - (uint32_t)sv;
 *
 * For sv = INT_MIN = 0x80000000:  0 - 0x80000000 = 0x80000000 = 2147483648.
 * For sv = -5         = 0xFFFFFFFB: 0 - 0xFFFFFFFB = 5.
 * The unsigned result is always the correct positive magnitude, and the
 * expression is well-defined by the C standard for every input. */

/* One hex digit (0..15) as a lowercase character. */
static char hex_char(uint32_t n)
{
    return (char)(n < 10 ? '0' + (char)n : 'a' + (char)(n - 10));
}

/* Write `v` as DECIMAL into buf (10-digit headroom + NUL headroom).
 * Returns the number of digits written (NOT including any terminator). */
static uint32_t u32_to_dec_str(uint32_t v, char *buf)
{
    char tmp[11];            /* 10 digits + slack */
    int i = 10;              /* write from the back */

    if (v == 0)
        tmp[--i] = '0';     /* special case: "0" */

    while (v != 0) {
        tmp[--i] = (char)('0' + (char)(v % 10));
        v /= 10;
    }

    /* The digits now sit in tmp[i..9] (the first write went into
     * tmp[9], so the highest index ever touched is 9). Copy them
     * forward into buf. */
    for (uint32_t j = i; j < 10; j++)
        buf[j - i] = tmp[j];
    return (uint32_t)(10 - i);
}

/* Write `v` as LOWERCASE HEX into buf, optionally zero-padded to
 * `pad_width` digits (0 = no padding). Always returns at least 1 digit
 * ("0" for v == 0). Max 8 digits; buf must hold 8 + 1.
 *
 * The reverse-write trick is the same as for decimal: nibbles peel off
 * low-to-high, so we fill tmp from the back and walk forward. */
static uint32_t u32_to_hex_str(uint32_t v, char *buf, uint32_t pad_width)
{
    char tmp[8];
    uint32_t i = 8;

    for (uint32_t n = 0; n < 8; n++)
        tmp[--i] = hex_char(v & 0xFu);   /* nibble 0 -> tmp[7], nibble 7 -> tmp[0] */

    /* Where do we start inside the 8-nibble field?
     *   - pad_width == 0: skip the leading zeros, but keep at least
     *     one digit ("0" for v == 0) -- that is the "%x" behavior.
     *   - pad_width >= 1: start exactly pad_width nibbles from the
     *     RIGHT, so the top of the field (zeros included) is emitted.
     *     pad_width is clamped to 1..8 by the caller (printk_hex). */
    if (pad_width == 0)
        while (i + 1 < 8 && tmp[i] == '0')
            i++;
    else
        i = 8 - pad_width;

    uint32_t len = 8 - i;
    for (uint32_t j = i; j < 8; j++)
        buf[j - i] = tmp[j];
    return len;
}

/* ===========================================================================
 * PART 7: PUBLIC API -- printk and printk_hex
 * ===========================================================================
 *
 * printk(fmt, ...)
 * ----------------------------------------------------------------------------
 * Supports EXACTLY the specifiers promised in include/kernel.h:
 *   %c  one character         %s  NUL-terminated string
 *   %x  lowercase hex, no padding
 *   %d  signed decimal       %u  unsigned decimal
 *   %%  a literal '%'
 *
 * The variadic mechanism is <stdarg.h>: va_start(va, fmt) snapshots the
 * register/stack state after `fmt`, and every va_arg pulls one argument
 * off the top, in reverse declaration order (cdecl: the last argument was
 * pushed onto the stack first). va_arg's second argument is the PROMOTED
 * type of the corresponding argument: `char` is promoted to `int` (a
 * vararg char arrives as a 32-bit int on i386, so we fetch int and
 * truncate back), and `int32_t`/`uint32_t` are `int`/`unsigned int`
 * themselves on this target -- no promotion, one register each. */

void printk(const char *fmt, ...)
{
    va_list ap;

    /* Lazy one-time serial init (see `serial_ready` above). The VGA
     * path needs no init -- the framebuffer is just memory and it is
     * already set up by the BIOS/GRUB. The UART, however, is in an
     * arbitrary state after the BIOS, and we must NOT trust it to be
     * 8N1@115200 with FIFOs enabled. Doing it here, once, on the first
     * printk call means every code path (gdt_install, stack_dump, the
     * shell) gets a ready serial port without each module having to
     * remember to call serial_init() itself. */
    if (!serial_ready) {
        serial_init();
        serial_ready = 1;
    }

    va_start(ap, fmt);

    for (const char *p = fmt; *p != 0; p++) {
        if (*p != '%') {
            console_putc(*p);
            continue;
        }

        /* Consume the '%'; *p is now the specifier (or 0 if the format
         * string ended with a bare '%'). */
        p++;
        switch (*p) {
        case 0:
            /* Stray '%' at the very end of the format string: print it
             * and stop. */
            console_putc('%');
            p--;                    /* p will be incremented by the loop */
            break;

        case '%':
            console_putc('%');
            break;

        case 'c':
            /* A char argument is PROMOTED to int in varargs; fetch the
             * 32-bit int and truncate back. */
            console_putc((char)(va_arg(ap, int)));
            break;

        case 's':
            for (const char *s = va_arg(ap, const char *); *s; s++)
                console_putc(*s);
            break;

        case 'd':
        case 'u': {
            uint32_t v;
            if (*p == 'u') {
                v = va_arg(ap, uint32_t);
            } else {
                /* %d: fetch as int32_t. If negative, print the sign
                 * FIRST (before the digits), then take the magnitude in
                 * the unsigned domain -- see the INT_MIN discussion
                 * above. 0u - (uint32_t)sv wraps modulo 2^32 and is
                 * well-defined for every sv, including INT_MIN. */
                int32_t sv = va_arg(ap, int32_t);
                if (sv < 0) {
                    console_putc('-');
                    v = 0u - (uint32_t)sv;
                } else {
                    v = (uint32_t)sv;
                }
            }
            char buf[11];
            uint32_t n = u32_to_dec_str(v, buf);
            for (uint32_t i = 0; i < n; i++)
                console_putc(buf[i]);
            break;
        }

        case 'x':
            /* Unpadded lowercase hex, minimum 1 digit ("0" for zero).
             * The 0 pad argument means "no leading-zero padding". */
            {
                uint32_t v = va_arg(ap, uint32_t);
                char buf[8];
                uint32_t n = u32_to_hex_str(v, buf, 0);
                for (uint32_t i = 0; i < n; i++)
                    console_putc(buf[i]);
            }
            break;

        default:
            /* An unknown specifier: print the '%' and the character so
             * the output is still readable (better than silently eating
             * the argument, which would desync the vararg stack). */
            console_putc('%');
            console_putc(*p);
            break;
        }
    }

    va_end(ap);
}

/*
 * printk_hex(value, width)
 * ----------------------------------------------------------------------------
 * Print `value` as lowercase hex, ZERO-PADDED to `width` digits (width
 * clamped to 1..8). Used for addresses, registers, and any value where
 * the width matters (column alignment in the stack dump, GDT summary,
 * magic numbers). Example:
 *
 *   printk_hex(0x800, 8)  ->  "00000800"
 *   printk_hex(0x07FF, 8) ->  "000007ff"
 *   printk_hex(0x0042, 4) ->  "0042"
 *
 * We format into a small stack buffer, then hand it back to printk("%s")
 * so that:
 *   - the lazy serial init runs on THIS path too (if the user's very
 *     first call is printk_hex, the UART still gets initialized), and
 *   - every byte still funnels through console_putc (VGA + serial, in
 *     lockstep) exactly like any other printk output.
 */
void printk_hex(uint32_t value, int width)
{
    /* Clamp the width to the range the hardware (and our 8-nibble
     * buffer) can express. width < 1 would mean "print nothing", which
     * is not useful; width > 8 would overflow the buffer. */
    if (width < 1) width = 1;
    if (width > 8) width = 8;

    char buf[9];            /* 8 hex digits + NUL */
    uint32_t n = u32_to_hex_str(value, buf, (uint32_t)width);
    buf[n] = 0;

    /* Route through printk so the lazy serial init fires (in case this
     * is the very first call) and every byte goes through
     * console_putc -- the screen and the serial mirror stay in lockstep
     * even on this path. */
    printk("%s", buf);
}

/* ===========================================================================
 * PART 8: A NOTE ON WHAT THIS FILE DOES NOT DO (yet)
 * ===========================================================================
 *
 * This file is deliberately small: two output destinations, six format
 * specifiers, a cursor, a scroll loop. It is the "Bare Bones" console
 * from the OSDev wiki -- the absolute minimum that makes the rest of the
 * kernel debuggable. What it intentionally does NOT do:
 *
 *   - No color per character (we always emit attribute 0x07). The
 *     attribute byte is there in every cell, so a "printk_color"
 *     variant is a ~20-line addition later, not a rework.
 *   - No tab expansion. '\b' and '\r' ARE handled (the shell's line
 *     editing depends on '\b'), but '\t' would need a column-tab
 *     policy; printk is a one-way "print this string" primitive.
 *   - No interrupt-driven UART. The LSR polling loop in serial_putc
 *     spins with interrupts disabled, which is correct for a kernel
 *     that keeps the CPU in a tight loop (boot, then shell). A real
 *     OS would wire IRQ4 and queue bytes in a ring buffer; that is a
 *     separate module and a separate set of lecture notes.
 *
 * The invariants this file guarantees (rely on them in other modules):
 *
 *   1. Every call to printk/printk_hex is atomic with respect to the
 *      screen: no other code path writes to the VGA buffer, and the
 *      cursor is always advanced to the next free cell.
 *   2. The screen and the serial mirror are byte-for-byte identical in
 *      order; the two destinations never disagree.
 *   3. The serial port is initialized exactly once, on the first
 *      printk call, and every subsequent call is "just print".
 * =========================================================================== */
