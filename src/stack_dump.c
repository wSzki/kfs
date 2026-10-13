/*
 * src/stack_dump.c -- the mandatory "print the kernel stack, human-friendly"
 * tool of KFS_2 (src file 6 of the project).
 * ============================================================================
 *
 * ROLE IN THE BOOT CHAIN
 * ----------------------
 * kmain() (src/kmain.c) calls stack_dump() as its second milestone:
 *
 *   kmain
 *    |-- gdt_install()   : our GDT at 0x800 (src/gdt.c + gdt_flush.asm)
 *    |-- stack_dump()    : THIS FILE -- hexdump the USED part of the stack
 *    `-- shell_run()     : debug shell; its "printstack" command re-enters
 *                          this same function, so the dump always shows the
 *                          stack exactly as it looks at the CALLING moment.
 *
 * WHY A STACK DUMP AT ALL?
 *   The stack is where C lives or dies on bare metal: every `call`,
 *   every local variable, every `push`/`pop` goes through SS:ESP. If the
 *   stack pointer is wrong, nothing else can be right, and the failure is
 *   silent (memory corruption far away from the cause) until it triple-
 *   faults. Printing [ESP, stack_top) is the cheapest possible proof that
 *   the stack is alive and behaving -- it is this project's answer to the
 *   subject's "human-readable kernel stack print" requirement, and the
 *   first diagnostic tool you reach for the rest of the course.
 *
 * CONCEPTS INTRODUCED HERE (read these pages, the comments assume you have)
 *   - https://wiki.osdev.org/Stack
 *         why the x86 stack grows downward, frames, ESP/EBP, calling
 *         conventions, and why the stack is sacred (interrupts!).
 *   - https://en.wikipedia.org/wiki/Cdecl
 *         how C passes arguments on the stack (right-to-left pushing,
 *         caller-cleanup) -- the exact convention boot.asm follows when it
 *         calls kmain.
 *   - https://gcc.gnu.org/onlinedocs/gcc/Extended-Assembly.html
 *         the GNU C inline-assembly reference: asm syntax, constraint
 *         strings ("=r", ...), and the %% escaping we use below.
 *
 * ----------------------------------------------------------------------------
 * THE x86 STACK GROWS DOWNWARD -- the single most important fact in this file
 * ----------------------------------------------------------------------------
 * On x86 a `push` FIRST decrements ESP and THEN writes. A stack therefore
 * lives at its HIGHEST address and is consumed TOWARDS lower addresses:
 *
 *       HIGH addresses
 *       |
 *       |  stack_top  =  stack_bottom + 16384
 *       |              (boot.asm: `mov esp, stack_top`)
 *       +--------------------------------------------------+
 *       |  [mbi_addr]     @ ESP+8                          |
 *       |  [magic]        @ ESP+4   == 0x2BADB002          |
 *       |  [ret addr]     @ ESP                             |
 *       +--------------------------------------------------+
 *          ^ ESP -- used region = [ESP, stack_top)
 *       |                                                      what
 *       |   16372 free bytes, zeroed .bss                      _start
 *       +--------------------------------------------------+  and the
 *       |                                                     CPU pushed:
 *       stack_bottom  (LOW addresses)                          1) mbi_addr (arg 2,
 *                                                              pushed FIRST, cdecl)
 *                                                              2) magic    (arg 1,
 *                                                              pushed second)
 *                                                              3) ret addr (by `call kmain`,
 *                                                              points at the _hang label)
 *
 * So the "used part" of the stack is the SMALL interval
 *
 *        [ESP, stack_top)   -- ESP inclusive, stack_top exclusive.
 *
 * At the very start of this project that used part is a handful of words:
 * the two arguments _start pushed (kmain's `magic` and `mbi_addr`) plus the
 * return address the CPU pushed for `call kmain` (it points at the `_hang`
 * label in boot.asm, because kmain never actually returns). Everything
 * above ESP is still the zeroed .bss -- which is exactly why a healthy
 * dump looks "dirty on top, clean below": any pattern in the lower rows is
 * something that has not happened yet (i.e. a bug).
 *
 * HONEST CAVEAT: stack_dump() itself is a `call`, so when this file reads
 * ESP it sees the stack AFTER kmain called us: one more word (stack_dump's
 * return address) and possibly this function's own frame are on top of the
 * three words above. The dump still starts at the rounded-down ESP and runs
 * to stack_top, so the three "founding" words always land a few rows
 * BELOW stack_top. The table prints low-to-high, so look at the LAST
 * rows of the output: 0x2BADB002 MUST be visible there, or the whole
 * boot chain is suspect.
 *
 * ----------------------------------------------------------------------------
 * HOW WE READ ESP/EBP: GNU C INLINE ASSEMBLY
 * ----------------------------------------------------------------------------
 * C has no way to name ESP; the compiler manages stack slots with its own
 * internal arithmetic and never exposes the raw pointer. So we drop to one
 * real machine instruction each:
 *
 *     __asm__("movl %%esp, %0" : "=r"(esp));
 *
 * Anatomy, constraint by constraint:
 *
 *   "movl %%esp, %0"
 *      - `movl`: the `l` suffix means 32-bit operands (we are i386; without
 *        an explicit size, `mov` is ambiguous).
 *      - `%%esp`: a single `%` inside an asm string starts an OPERAND
 *        reference (`%0`, `%1`, ...) to the compiler. To print a LITERAL
 *        register name we must escape it: `%%esp`, `%%ebp`, `%%eax`...
 *        This is why every kernel asm snippet looks "doubly percented".
 *      - `%0`: the first operand, defined by the constraint string below.
 *
 *   : "=r"(esp)
 *      One OUTPUT operand (the `:` separates inputs from outputs; here we
 *      have zero inputs). `"=r"` reads: "give me a writable general-
 *      purpose REGISTER, and the asm will WRITE the result into it". The
 *      `=` means "write-only" -- the compiler may pick the register but
 *      must not assume the asm reads its old contents, and we (the C code
 *      below) are now guaranteed the value lands in `esp`. If we forgot
 *      the `=`, the compiler treats the asm as not producing a value and
 *      `esp` keeps garbage: a classic silent bug, the same family as
 *        __asm__("movl %%esp, %0" : "r"(esp));   // WRONG, no '=': the
 *        operand is input-only, the store is meaningless to the compiler.
 *
 *   No `volatile` needed here: a `movl` whose output operand is later used
 *   cannot be deleted or reordered away; volatile would only forbid
 *   useful reordering. (The `outb` in kernel.h uses volatile because the
 *   out instruction has no operands at all, so the compiler sees a no-op.)
 *
 * EBP: on modern compilers EBP is just a "base pointer" register that the
 * C compiler OPTIONALLY uses for frames. If -O0 builds a frame, EBP inside
 * stack_dump() is the saved entry-time ESP (pointing just below the top
 * row); if -fomit-frame-pointer is used, or for the very first call, EBP
 * carries whatever GRUB/_start left in it -- _start never sets it. This
 * tool does not try to decide which: it merely MARKS EBP's row when EBP
 * falls inside the dumped range and stays silent otherwise.
 *
 * ----------------------------------------------------------------------------
 * THE DUMP LAYOUT (what you see on screen)
 * ----------------------------------------------------------------------------
 * One row = 16 bytes = 4 dwords, matching the classic `hexdump -C` style:
 *
 *   00107fa0  00000000 02b0ad2b 00000001 00000000  |......+.......|
 *   +----------+  -------------------------------+
 *    address     4 dwords, each 8 zero-padded hex digits (printk_hex)
 *   (8 digits)
 *    |      + 16 raw bytes, printable 0x20-0x7E shown as-is, else '.'
 *
 * Markers: the row CONTAINING the current ESP gets a "  <- ESP" suffix
 * (the row containing EBP gets "  <- EBP"; both suffixes print if they
 * share a row). Before the table, a summary line:
 *
 *   ESP=0x00107fa0 EBP=0x00107fa4
 *
 * LITTLE-ENDIAN -- THE MOST CONFUSING PART OF EVERY HEXDUMP (read twice)
 * -----------------------------------------------------------------------
 * The i386 is LITTLE-ENDIAN: a 32-bit value is stored with its LOW byte at
 * the LOWEST address. So the dword 0x00000041 ('A') at address A lives in
 * memory as byte-by-byte (address order):
 *
 *        address :   A      A+1      A+2      A+3
 *        byte    :  0x41   0x00   0x00   0x00
 *
 * Now store the string "ABCD" and read it back as one dword:
 *
 *        address :  0x41('A') 0x42('B') 0x43('C') 0x44('D')
 *        dword   :  0x44434241
 *
 * The HEX column shows the value in its NATURAL order (44434241, most
 * significant digit left) while the ASCII column shows the bytes in
 * ADDRESS order -- so the two look like each other "reversed". Every hex
 * dump in existence (this one, `hexdump`, GDB's `x/8wx`) displays the hex
 * in big-endian style but the ASCII in byte order, and the mismatch is
 * precisely the little-endian layout. When your dump shows ASCII "ABCD",
 * the adjacent hex word MUST read 44434241, not 41424344.
 *
 * ----------------------------------------------------------------------------
 * ALIGNMENT, TRUNCATION, AND THE 80-COLUMN WRAP (design decisions)
 * ----------------------------------------------------------------------------
 * - dump start = ESP rounded DOWN to a 16-byte boundary. The stack pointer
 *   is only 4-byte aligned (each push moves it by 4), so without rounding
 *   the ASCII column would start mid-dword and no row would ever line up
 *   on a dword. stack_top is 16-aligned (boot.asm: `alignb 16` + 16 KiB),
 *   so after rounding, every row spans whole dwords AND the last row ends
 *   exactly at stack_top.
 * - hard cap of DUMP_MAX_ROWS rows (64 * 16 = 1 KiB): at boot the used
 *   region is a dozen words, but if you grow the kernel and forget this
 *   tool, an unbounded dump would scroll the screen for minutes. If the
 *   region is ever larger, print "... (truncated ...)" instead.
 * - ROW WIDTH: a full row is ~75 characters: 8 (address) + 4 x 9 (hex
 *   dwords) + 20 (ASCII column). That fits the 80-column VGA text mode
 *   without wrapping -- one of the reasons a 16-byte row carries 4
 *   dwords rather than 8.
 *
 * FREESTANDING RULES (project-wide): no libc, only stdint/stddef/stdarg
 * via kernel.h; output ONLY through printk/printk_hex (VGA + serial, per
 * include/kernel.h section 5). No VGA addresses, no putchar, no printf.
 * ============================================================================
 */

#include "../include/kernel.h"

/* Row geometry: one row covers ROW_BYTES bytes and prints exactly
 * ROW_BYTES / sizeof(uint32_t) dwords -- deriving it instead of writing
 * "8" by hand is the guard against the classic out-of-bounds bug: a
 * dword-count that exceeds bytes/4 would make the dword loop READ PAST
 * the end of the row (and, on the final row, past stack_top). */
#define ROW_BYTES      16u
#define ROW_WORDS      (ROW_BYTES / sizeof(uint32_t))   /* = 4 */

/* Hard cap on rows: 64 rows x 16 bytes = 1 KiB of stack at most. At boot
 * the used region is a handful of words, so this cap is pure insurance
 * against a runaway dump if the kernel ever grows a deep call chain. */
#define DUMP_MAX_ROWS  64u

void stack_dump(void)
{
    uint32_t esp, ebp;                /* captured pointers, step 1 below  */
    uint32_t dump_start, dump_end;     /* the [start, end) interval to print*/
    uint32_t row_addr;                 /* running row address              */
    uint32_t rows_total, rows_printed; /* for the truncation note          */
    uint32_t word;                     /* one reassembled dword per loop   */
    uint8_t *row;                     /* byte view of the current row      */
    char ascii[ROW_BYTES + 1];        /* 16 chars + NUL for the %s print   */
    int i;

    /* ------------------------------------------------------------------
     * STEP 1 -- CAPTURE ESP AND EBP BEFORE THE COMPILER MOVES THINGS
     * ------------------------------------------------------------------
     * The two instructions documented in the header ("HOW WE READ
     * ESP/EBP"). Done FIRST, before any local variable is assigned, so
     * the values we print are the ones that were live at entry -- not
     * the addresses of this function's own stack slots, which the
     * compiler will happily hand out as it initializes the locals.
     * (`mov esp,%esp`-style self-movs are not a thing; we move OUT of
     * the pointer into a register operand the compiler allocated.)
     */
    __asm__("movl %%esp, %0" : "=r"(esp));
    __asm__("movl %%ebp, %0" : "=r"(ebp));

    /* ------------------------------------------------------------------
     * STEP 2 -- COMPUTE THE USED REGION [dump_start, dump_end)
     * ------------------------------------------------------------------
     * The stack grows DOWN, so the used region is the top slice:
     * everything from the last-written byte up to stack_top. stack_top
     * is an `extern` symbol from boot.asm (kernel.h section 4): ONE PAST
     * the highest usable byte, i.e. an exclusive end, like the second
     * index of a C array.
     */
    dump_end   = (uint32_t)stack_top;      /* exclusive end                 */
    dump_start = esp & ~(ROW_BYTES - 1u);  /* round DOWN to a 16-byte boundary */

    /*
     * OVERFLOW GUARD (classic-bug callout): if code ran past the bottom
     * of our reserved 16 KiB, ESP is BELOW stack_bottom and the "used
     * region" is now walking through some unrelated .bss section (or
     * wrapping the 4 GiB address space -- do not think about that). In
     * 32-bit there is no hardware guard, so the OS must notice: clamp
     * the walk to the reserved region and say so LOUDLY. A real kernel
     * would also keep its own `stack_guard` canary for this; here, a
     * visible warning is the whole mechanism.
     *
     * (In a healthy boot the clamp never fires: the used region at
     * kmain entry is under 64 bytes on a 16 KiB stack.)
     */
    if (dump_start < (uint32_t)stack_bottom) {
        printk("!! stack_dump: WARNING -- ESP=0x");
        printk_hex(esp, 8);
        printk(" is BELOW stack_bottom (");
        printk_hex((uint32_t)stack_bottom, 8);
        printk("): the stack OVERFLOWED its 16 KiB region; "
               "clamping the walk to the reserved area.\n");
        dump_start = (uint32_t)stack_bottom;  /* 16-aligned already (alignb 16) */
    }

    /* Both bounds are now 16-aligned (stack_top is too, see boot.asm),
     * so the interval length is a multiple of 16 and the row count is
     * exact. At boot this number is tiny: ~3 pushed words, rounded up
     * to one or two 16-byte rows. */
    rows_total = (dump_end - dump_start) / ROW_BYTES;

    /* ------------------------------------------------------------------
     * STEP 3 -- SUMMARY LINE, THEN THE TABLE
     * ------------------------------------------------------------------
     * The spec's exact summary format. Note the 0x prefix + 8-digit
     * fields: printk_hex pads to `width` digits but does NOT print a
     * prefix, so the "0x" is a literal here.
     */
    printk("Stack dump: [");
    printk_hex(dump_start, 8);
    printk("..");
    printk_hex(dump_end, 8);
    printk(") = ");
    printk("%u bytes in use, %u rows\n",
           (unsigned)(dump_end - dump_start), (unsigned)rows_total);
    printk("ESP=0x");
    printk_hex(esp, 8);
    printk(" EBP=0x");
    printk_hex(ebp, 8);
    printk("\n");

    /*
     * DEGENERATE CASE: rows_total == 0 happens only when ESP sits
     * exactly on stack_top -- i.e. not a single byte has ever been
     * pushed. There is no used region to walk, so instead of printing a
     * row that pokes past the reserved area, say so and stop. (This can
     * never actually trigger in this project: kmain calls us, and a
     * `call` itself pushes a return address, so the region is >= 8
     * bytes by the time we get here.)
     */
    if (rows_total == 0) {
        printk("(empty: ESP is exactly at stack_top -- nothing has been "
               "pushed yet)\n");
        return;
    }
    /* ------------------------------------------------------------------
     * STEP 4 -- THE ROWS: address | 4 hex dwords | ASCII | markers
     * ------------------------------------------------------------------
     * One row at a time, low to high. Each row prints:
     *   (a) the row's START address, 8 zero-padded digits
     *   (b) 4 dwords (16 bytes), each 8 zero-padded digits, space-separated
     *   (c) a "|" ASCII column of the raw 16 bytes, then "|"
     *   (d) a "  <- ESP" / "  <- EBP" marker when that value lives in
     *       THIS row (row range is [row_addr, row_addr+16), ESP points
     *       INTO the last-written word, so the containment test is the
     *       half-open interval on the row that contains ESP's address)
     */
    rows_printed = 0;
    for (row_addr = dump_start;
         row_addr < dump_end && rows_printed < DUMP_MAX_ROWS;
         row_addr += ROW_BYTES)
    {
        row = (uint8_t *)row_addr;
        const uint32_t row_end = row_addr + ROW_BYTES;

        /* (a) address column -- 8-digit hex, the row's low address */
        printk_hex(row_addr, 8);

        /* (b) the 4 dwords of this 16-byte row, reassembled byte-by-byte.
         *
         * We build each dword from the four bytes deliberately instead of
         * `*(uint32_t*)row` even though the address IS 4-aligned: the
         * shift/shift/shift/or assembly below IS the little-endian rule
         * made visible -- byte 0 contributes the value's LOW byte:
         *
         *      word = row[0] | row[1]<<8 | row[2]<<16 | row[3]<<24
         *
         * so the hex you print matches the byte order you can inspect in
         * the ASCII column next to it. Casting the pointer away and
         * reading a uint32_t would produce the same number but hide the
         * very fact this file exists to teach. */
        for (i = 0; i < (int)ROW_WORDS; i++) {
            word =  (uint32_t)row[4 * i]
                 | ((uint32_t)row[4 * i + 1] << 8)
                 | ((uint32_t)row[4 * i + 2] << 16)
                 | ((uint32_t)row[4 * i + 3] << 24);
            printk(" ");
            printk_hex(word, 8);
        }

        /* (c) ASCII column: printable 0x20..0x7E as-is, everything else
         * (high-bit bytes, control codes, NULs, the zeroed .bss) as '.'
         * so the row's width never shifts. Built into `ascii` and printed
         * with one %s -- the contract's format set has no "%c%c...16x",
         * and one call is both shorter and one less format-parse pass
         * through printk per row. Remember the little-endian twist above:
         * a word shown as 44434241 renders "ABCD" here. */
        for (i = 0; i < (int)ROW_BYTES; i++) {
            ascii[i] = (row[i] >= 0x20 && row[i] <= 0x7E) ? (char)row[i] : '.';
        }
        ascii[ROW_BYTES] = '\0';
        printk("  |%s|", ascii);

        /* (d) markers. Both are half-open [row_addr, row_end) tests: a
         * row "contains" a pointer P iff P is in [row_addr, row_addr+16).
         * ESP lives in the FIRST row (we rounded DOWN), EBP only when it
         * ever falls inside the walked range at all. Printing both
         * suffixes back to back is correct when a single row holds both
         * -- and at -O0, where the compiler DOES use EBP as frame
         * pointer, that is exactly what you will see on row 1. */
        if (esp >= row_addr && esp < row_end)
            printk("  <- ESP");
        if (ebp >= row_addr && ebp < row_end)
            printk("  <- EBP");
        printk("\n");

        rows_printed++;
    }

    /* ------------------------------------------------------------------
     * STEP 5 -- TRUNCATION NOTE (the DUMP_MAX_ROWS cap, header above)
     * ------------------------------------------------------------------
     * If the used region outgrew the cap, say exactly how much was not
     * printed. Never fail silently: a truncated dump that looks complete
     * is worse than one that admits it. */
    if (rows_printed < rows_total) {
        printk("... (truncated: ");
        printk("%u of %u rows shown, capped at DUMP_MAX_ROWS=",
               (unsigned)rows_printed, (unsigned)rows_total);
        printk("%u; %u rows not shown)\n",
               (unsigned)DUMP_MAX_ROWS,
               (unsigned)(rows_total - rows_printed));
    }

    /* And with that the "human-friendly kernel stack" requirement of the
     * subject is done: addresses, values, raw bytes, and the live
     * pointers -- enough to prove, on a real run, that ESP pointed at
     * the three founding words, the magic read back as 0x2BADB002, and
     * the return address pointed home at _hang. */
}
