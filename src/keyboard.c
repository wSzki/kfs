/*
 * src/keyboard.c -- polled PS/2 keyboard driver (NO interrupts anywhere).
 * ============================================================================
 *
 * ROLE IN THE BOOT CHAIN
 * ----------------------
 *   kmain -> shell_run -> keyboard_getchar (this file)
 *
 * shell_run() (src/shell.c) needs keystrokes to drive the debug shell.
 * A real OS reads the keyboard from an INTERRUPT handler: the CPU does
 * `sti`, waits in `hlt`, and IRQ1 (PS/2) wakes it via the 8259A PIC.
 * That pipeline needs an IDT, PIC remapping, and a proper ISR -- none of
 * which KFS_2 has yet. So instead we do the one honest thing a kernel
 * without interrupts can do: POLL.
 *
 * WHY POLLING IS ENOUGH HERE
 * --------------------------
 *   - No IDT:      we never build one, so `sti` would just raise a
 *                  double fault on the first IRQ. There is nothing to
 *                  vector into.
 *   - No PIC:      IRQ1 is still wired straight into the 8259A #1's
 *                  fixed vector 0x09, which points into GRUB's/BIOS's
 *                  territory -- we must not let it fire.
 *   - No handlers: the whole point of the interrupt-free design is that
 *                  there are none. The alternative lives in the OSDev
 *                  wiki's "PS/2 Keyboard" and "8259 PIC" pages: set up a
 *                  256-entry IDT, remap the PIC, write isr1(), and stash
 *                  scancodes into a ring buffer the shell reads from.
 *                  KFS_2 keeps that for a later milestone.
 *   - The cost of polling is CPU time: our busy-wait loop spins the CPU
 *     while waiting for a keypress. For a debug shell with no scheduler,
 *     no timers and no threads, "the CPU spins doing nothing" is exactly
 *     the same as "there is nothing to run". A later kernel with a
 *     scheduler MUST switch to interrupts -- a spinning task would starve
 *     every other thread.
 *
 * THE 8042 KEYBOARD CONTROLLER (the chip between CPU and keyboard)
 * -----------------------------------------------------------------
 * The PS/2 keyboard does not have a memory address; it is addressed
 * through TWO 8-bit I/O ports of the 8042 controller:
 *
 *   port 0x60  DATA     inb(0x60)  pops a byte out of the 8042's output
 *                                  buffer (that is where scancodes land)
 *   port 0x64  STATUS   inb(0x64)  returns the status register.
 *
 * Status register bits (the only one we use):
 *
 *   bit 0   OBF: output buffer full. 1 = a scancode is waiting in 0x60.
 *
 * So the read protocol is:
 *
 *   while (inb(0x64) & 0x01) == 0;   /* wait: no scancode yet   */
 *   uint8_t code = inb(0x60);        /* now it is                */
 *
 * SCANCODE SET 1 -- what the keyboard actually sends
 * ---------------------------------------------------
 * A PS/2 keyboard sends "scancodes", not ASCII: 0x1E means "key Q was
 * pressed", regardless of what the OS maps that to. One subtle point
 * that trips up beginners:
 *
 *   scancode & 0x80  == 0  -> "MAKE" (key pressed down)
 *   scancode & 0x80  != 0  -> "BREAK" (key released)
 *
 * Every key press is TWO bytes: a make code, then (when you let go) its
 * break code = make code | 0x80. 0x1E = Q pressed, 0x9E = Q released.
 * We only act on makes; breaks are used for exactly one thing: the
 * shift flags (below).
 *
 * SHIFT STATE
 * -----------
 * Shift itself sends a scancode too: 0x2A (left) and 0x36 (right) are
 * their make codes. We keep a STATIC (file-local) flag:
 *
 *   make  of 0x2A/0x36 -> shifted = 1
 *   break of 0x2A/0x36 -> shifted = 0
 *
 * Because makes of the character keys always arrive AFTER the shift
 * make (you press shift, then the letter), the flag is already correct
 * by the time a letter key is translated. A real kernel would also
 * handle Caps Lock and the 57-key extended scancodes (the 0xE0 prefix
 * byte); KFS_2 ignores both.
 *
 * THE TRANSLATION TABLE
 * ---------------------
 * `sc2c` is a plain byte array, 256 entries: the LOW 128 hold the
 * UNSHIFTED character for scancodes 0x00..0x7F, and the HIGH 128 hold
 * the SHIFTED character at the same indices offset by | 0x80 -- that
 * is why it is "indexable": translation is one table lookup, no
 * if-chains.
 * keyboard_getchar() picks the half with the current shift flag:
 *
 *   sc2c[code | (shifted ? 0x80 : 0)]
 *
 * Unlisted slots are 0, and 0 (NUL) is our "ignore this code" marker,
 * so unknown scancodes silently fall through.
 *
 * Further reading (read BOTH before touching this file):
 *   - https://wiki.osdev.org/PS/2_Keyboard
 *   - https://wiki.osdev.org/"8042"_PS/2_Controller
 * ============================================================================
 */

#include "../include/kernel.h"

/* Port addresses of the 8042 keyboard controller (see the big header). */
#define PS2_DATA  0x60   /* inb: pop a scancode / outb: send a command */
#define PS2_STAT  0x64   /* inb: status register                     */

#define OBF  0x01        /* status bit 0: output buffer full         */

/* Left/right shift MAKE codes, and the BREAK codes (make | 0x80).     */
#define SC_LSHIFT_MAKE   0x2A
#define SC_RSHIFT_MAKE   0x36
#define SC_LSHIFT_BREAK  (0x2A | 0x80)
#define SC_RSHIFT_BREAK  (0x36 | 0x80)

/* ============================================================================
 * sc2c: scancode -> character, scancode set 1.
 *
 * Layout: 128 entries for the UNSHIFTED meanings (codes 0x00-0x7F) and
 * 128 for the SHIFTED meanings (same indices, offset by | 0x80).
 *
*   0x02..0x0D  unshifted: '1'..'9','0','-','='
*                shifted: '!','@','#','$','%','^','&','*','(',')',
*                '_','+'   (the classic "shift + digit" row)
*   0x0E        '\b'  (BACKSPACE; its break code is 0x8E)
*   0x0F        '\t'  (TAB -- left at 0 = ignored, the shell has no
*                tab handling)
*   0x1C        '\n'  (ENTER)
*   0x1E..0x26  'a','s','d','f','g','h','j','k','l'  (the home row)
*   0x27..0x29  ';', '\'', '`'   shifted: ':', '"', '~'
*   0x2B        '\'           shifted: '|'
*   0x2C..0x32  'z','x','c','v','b','n','m'
*   0x33..0x35  ',', '.', '/'  shifted: '<', '>', '?'
*   0x39        ' '  (SPACE -- easy to forget, the shell needs it!)
*
* Everything left at 0 stays 0 = "ignore". The shifted half mirrors the
* unshifted half, uppercasing letters and symbol-shifting the number
* row. A few codes (caps lock 0x3A, scroll lock, print screen, the
* 0xE0-prefixed extended keys, ...) are deliberately left as 0: the
* debug shell has nothing to do with them.
* ========================================================================= */
static const char sc2c[256] = {
    /* number row */
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x0C] = '-', [0x0D] = '=',
    [0x0E] = '\b',
    [0x1C] = '\n',
    /* top letter row: q w e r t y u i o p */
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p',
    [0x1A] = '[', [0x1B] = ']',
    /* home letter row: a s d f g h j k l */
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l',
    [0x27] = ';', [0x28] = '\'', [0x29] = '`',
    [0x2B] = '\\',
    /* bottom letter row: z x c v b n m */
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm',
    [0x33] = ',', [0x34] = '.', [0x35] = '/',
    [0x39] = ' ',

    /* Shifted half: identical indices, offset by | 0x80. Letters are
     * uppercased; the number row becomes its symbols. */
    [0x02 | 0x80] = '!', [0x03 | 0x80] = '@', [0x04 | 0x80] = '#',
    [0x05 | 0x80] = '$', [0x06 | 0x80] = '%', [0x07 | 0x80] = '^',
    [0x08 | 0x80] = '&', [0x09 | 0x80] = '*', [0x0A | 0x80] = '(',
    [0x0B | 0x80] = ')', [0x0C | 0x80] = '_', [0x0D | 0x80] = '+',
    /* top row shifted */
    [0x10 | 0x80] = 'Q', [0x11 | 0x80] = 'W', [0x12 | 0x80] = 'E',
    [0x13 | 0x80] = 'R', [0x14 | 0x80] = 'T', [0x15 | 0x80] = 'Y',
    [0x16 | 0x80] = 'U', [0x17 | 0x80] = 'I', [0x18 | 0x80] = 'O',
    [0x19 | 0x80] = 'P',
    [0x1A | 0x80] = '{', [0x1B | 0x80] = '}',
    /* home row shifted */
    [0x1E | 0x80] = 'A', [0x1F | 0x80] = 'S', [0x20 | 0x80] = 'D',
    [0x21 | 0x80] = 'F', [0x22 | 0x80] = 'G', [0x23 | 0x80] = 'H',
    [0x24 | 0x80] = 'J', [0x25 | 0x80] = 'K', [0x26 | 0x80] = 'L',
    [0x27 | 0x80] = ':', [0x28 | 0x80] = '"', [0x29 | 0x80] = '~',
    [0x2B | 0x80] = '|',
    /* bottom row shifted */
    [0x2C | 0x80] = 'Z', [0x2D | 0x80] = 'X', [0x2E | 0x80] = 'C',
    [0x2F | 0x80] = 'V', [0x30 | 0x80] = 'B', [0x31 | 0x80] = 'N',
    [0x32 | 0x80] = 'M',
    [0x33 | 0x80] = '<', [0x34 | 0x80] = '>', [0x35 | 0x80] = '?',
};

/*
 * Current shift state: 1 while EITHER shift key is physically held.
 * `static`: file-local, so the shell can never write to it by accident;
 * the driver is the ONLY writer.
 */
static int shifted = 0;

/*
 * keyboard_getchar() -- the shell's one input primitive.
 *
 * BLOCKING busy-poll: spin until the 8042's status bit 0 (OBF) says a
 * scancode is waiting, read it from 0x60, translate, and return the
 * character -- or -1 when the byte was an ignored release/unknown
 * scancode the caller should simply discard.
 *
 * WHY SPIN AND NOT SLEEP?  The tempting "save CPU cycles" trick is to
 * issue `hlt` in the loop. `hlt` stops the CPU until an interrupt
 * arrives -- but KFS_2 has NO IDT and interrupts are OFF (`cli` in
 * boot.asm, never `sti`d). With `hlt` and no pending IRQ the CPU
 * simply sleeps FOREVER: no IRQ will ever fire because we did not wire
 * one up. So with interrupts off the only options are "spin" or "sleep
 * until the machine is unplugged". Spinning is the lesser evil, and
 * here -- no scheduler, no threads -- it costs the system nothing.
 */
int keyboard_getchar(void)
{
    while (1) {
        /* Wait until OBF (status bit 0) is set: a scancode is waiting.
         * io_wait() between checks is not needed here (a 1us delay is
         * irrelevant at the timescale of keypresses) -- the loop is
         * just "re-check status, re-check status, ...".                */
        if ((inb(PS2_STAT) & OBF) == 0)
            continue;

        uint8_t code = inb(PS2_DATA);   /* pop the waiting scancode   */

        /*
         * Break code: the 0x80 bit is set. The ONE thing breaks give
         * us is the shift flag: a shift BREAK clears it. A letter
         * release carries no information a debug shell needs, so it
         * is ignored: we return -1 and let the caller ask again.
         */
        if (code & 0x80) {
            if (code == SC_LSHIFT_BREAK || code == SC_RSHIFT_BREAK)
                shifted = 0;
            return -1;
        }

        /*
         * Make codes that steer the shift flag. They produce no
         * character, so after updating the flag we -1 out and let
         * the caller keep waiting for the next byte.
         */
        if (code == SC_LSHIFT_MAKE || code == SC_RSHIFT_MAKE) {
            shifted = 1;
            return -1;
        }

        /*
         * Translate via the table: the high half (| 0x80) is the
         * shifted mapping. A 0 result means "code we do not know"
         * (a release we were not tracking, a key with no entry).
         * The contract is: RETURN -1 in that case -- the caller
         * (the shell) discards the -1 and asks for the next byte.
         */
        char c = sc2c[code | (shifted ? 0x80 : 0)];
        if (c == 0)
            return -1;

        return (unsigned char)c;
    }
}
