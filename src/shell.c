/*
 * src/shell.c -- the KFS_2 debug shell (the "bonus" part of the project).
 * ============================================================================
 *
 * ROLE IN THE BOOT CHAIN
 * ----------------------
 *   kmain -> printk -> gdt_install -> stack_dump -> shell_run  (THIS file)
 *
 * After the GDT is installed and the stack has been dumped (the two
 * mandatory deliverables), kmain() hands over to shell_run() and is
 * never reached again: this function is an INFINITE loop and is the
 * last C code the machine ever executes.
 *
 * WHY A BLOCKING SHELL IS ACCEPTABLE HERE
 * ----------------------------------------
 * A shell that simply BLOCKS waiting for a keypress sounds like a bug
 * in a kernel -- in a real OS it would be: it would freeze every other
 * task while it waits on the keyboard. But KFS_2 has NO scheduler and
 * NO threads: there is literally no one else to run. When nothing is
 * happening, the only useful thing a bare kernel can do IS wait on
 * input. The busy-poll in keyboard_getchar() (src/keyboard.c) spins
 * the CPU, and in a multi-tasking kernel that spin would starve other
 * tasks -- so the moment a scheduler lands, this shell must move to an
 * interrupt-driven keyboard driver and a blocking "wait for input"
 * that actually yields the CPU. Not today.
 *
 * THE LOOP, STEP BY STEP
 * ----------------------
 *   print "kfs2> "
 *   loop: c = keyboard_getchar()
 *       '\b' -> erase the last echoed char, if the line is not empty
 *       '\n' -> command complete: dispatch it
 *       else -> echo it, store it into line[]
 *   dispatch: help / printstack / reboot / halt / <unknown>
 *   back to "kfs2> "
 *
 * Further reading:
 *   - https://wiki.osdev.org/PS/2_Keyboard          (what we are polling)
 *   - https://wiki.osdev.org/"8042"_PS/2_Controller (the port pair 0x60/0x64)
 * ============================================================================
 */

#include "../include/kernel.h"

/* ============================================================================
 * LINE EDITING STATE
 *
 * `line` is where the command being typed accumulates; `pos` is the
 * index of the next free slot (i.e. the current length, since we keep
 * line NUL-terminated).
 *
 * WHY 128? A line buffer in a kernel should be a constant the author
 * chose, not "whatever the compiler likes". 127 chars + NUL fits in a
 * static 128-byte array and is long enough for "printstack", the
 * longest KFS_2 command.
 *
 * THE OVERFLOW GUARD (repeated at the store site below):
 *     if (pos < 127)
 *     ...
 * `pos` can reach at most 127, meaning 127 characters are stored at
 * indices 0..126, leaving slot 127 for the terminating NUL.
 * Without the guard, typing 128 chars would march the write
 * PAST the array -- in a kernel that is not a crash, it is
 * silent corruption of whatever .bss/.data follows `line`. In an OS
 * you would also cap the *echo* at the screen width (80 cols); KFS_2
 * lets long lines wrap, which is harmless here.
 * ========================================================================= */
static char line[128];
static int pos = 0;

/*
 * str_eq: the one string primitive the shell needs. We cannot use
 * <string.h> -- the kernel is freestanding, and <string.h> is NOT one
 * of the headers the C standard guarantees to exist in a freestanding
 * environment (only <stdint.h>, <stddef.h>, <stdarg.h>, <stdbool.h>
 * and friends are). Three lines, no surprises:
 * walk both strings until either ends; equal only if they end
 * together.
 */
static int str_eq(const char *a, const char *b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

/*
 * print_char: echo ONE character to the console. printk() is the only
 * console primitive the contract gives us, and it always appends --
 * which is exactly what echoing is: one character at a time, no
 * formatting, no '\n'.
 *
 * Why not print("%c") via printk every time and skip this? You can,
 * and this wrapper exists mostly so the "echo" call sites in
 * shell_run() read like what they are, and so that if the console
 * gains a single-char fast path later, it changes in ONE place.
 */
static void print_char(char c)
{
    printk("%c", c);
}

/* ============================================================================
 * THE COMMANDS
 * ========================================================================= */

/*
 * cmd_help: list what exists. The shell is the whole point of the
 * bonus part, so the help text doubles as the user manual: it tells
 * you what each command DOES, not just its name.
 */
static void cmd_help(void)
{
    printk("commands:\n");
    printk("  help      -- show this list\n");
    printk("  printstack -- dump the kernel stack, address / hex / ASCII\n");
    printk("  reboot    -- reset the machine via the 8042 (port 0x64)\n");
    printk("  halt      -- stop the CPU forever (cli + hlt loop)\n");
    printk("  (any other word -> \"unknown command\")\n");
}

/*
 * cmd_printstack: delegates to stack_dump() from the contract
 * (declared in kernel.h, implemented in src/stack_dump.c). Re-running
 * it here is meaningful -- the shell's own frames have piled onto the
 * stack since the dump at boot time, so each invocation shows a
 * (slightly) longer used region: a nice live demo of stack growth.
 */
static void cmd_printstack(void)
{
    stack_dump();
}

/*
 * cmd_reboot: reset the machine.
 *
 * THE CLASSIC TRICK: the 8042 keyboard controller has a RESET pin
 * wired to the CPU. Writing 0xFE to port 0x64 (its command port)
 * makes the 8042 pulse that pin for ~5 microseconds -- the CPU
 * reboots exactly as if you had yanked power. One byte, no drivers.
 * This is literally how countless bare-metal kernels "reboot".
 *
 * THE ALTERNATIVE you have read about: force a TRIPLE FAULT
 * deliberately (write to a CS of 0, jump to code segment 0, or smash
 * the stack pointer). Two faults already pending -> third fault ->
 * the chip reset itself. It works, but it is uglier, and on modern
 * virtualized hardware a triple fault may take the whole VM down
 * instead of resetting it. The 0xFE pulse is the polite version.
 *
 * AFTER outb we also spin in hlt: some emulators (and some real
 * motherboards with slow reset circuitry) take a beat; the hlt loop
 * guarantees that if the reset pulse does not take, the CPU at least
 * does not fall through into garbage. With interrupts OFF, hlt here
 * means "sleep until the reset pulse lands" -- which is exactly the
 * sleep we wanted.
 */
static void cmd_reboot(void)
{
    printk("rebooting via 8042 reset pulse...\n");
    outb(0x64, 0xFE);           /* pulse the CPU reset line         */
    for (;;) {
        __asm__ volatile ("hlt"); /* fallback: sleep until reset    */
    }
    /* (unreachable in practice: the reset pulse lands first)       */
}

/*
 * cmd_halt: stop the CPU, do not reset.
 *
 * THE TWO INSTRUCTIONS, IN ORDER:
 *   cli  -- clear the IF flag: no more interrupts will EVER be taken.
 *           (They are already off since boot.asm; the cli is
 *           belt-and-braces, and it documents intent.)
 *   hlt  -- stop the core. The CPU draws almost no power and does
 *           NOTHING until an interrupt arrives. With cli just issued,
 *           no interrupt will EVER arrive -> the machine is stopped,
 *           frozen in place. Memory is intact, no reset happened.
 *
 * This is the cleanest "power off" a bare kernel can offer without
 * talking to an ACPI table (a real OS hibernates via S4/S5, which
 * requires the ACPI firmware to agree -- KFS_2 would just have to
 * trust the firmware, which we do not want to do in a school
 * project). The user's "off switch" is closing the QEMU window.
 */
static void cmd_halt(void)
{
    printk("halting (cpu will sleep forever; close QEMU to exit)\n");
    __asm__ volatile ("cli");    /* no interrupts, ever              */
    for (;;) {
        __asm__ volatile ("hlt"); /* and sleep. Forever.             */
    }
    /* (unreachable)                                                    */
}

/* ============================================================================
 * shell_run: the top-level loop (contract entry point, never returns).
 * ========================================================================= */
void shell_run(void)
{
    /* Banner: make it obvious we have left "boot-time prints" and
     * entered interactive mode. The banner is also the first time
     * the user is invited to type something.                       */
    printk("\nKFS_2 shell -- type \"help\" for the command list\n");

    while (1) {
        /* ---- Prompt ------------------------------------------------
         * Printed before EVERY readline, exactly like /bin/sh. The
         * prompt is how the user knows "you can type now".         */
        printk("kfs2> ");

        /* Reset the line buffer for the next command.               */
        pos = 0;
        line[0] = '\0';

        /* ---- Read one line, one character at a time --------------- */
        for (;;) {
            int c = keyboard_getchar();

            /*
             * -1: keyboard_getchar() reports a byte it could not
             * decode (an unknown scancode). There is nothing to echo
             * and nothing to store -- just wait for the next key.
             */
            if (c < 0)
                continue;

            char ch = (char)c;

            if (ch == '\b') {
                /*
                 * ERASE: vga_putc() in src/printk.c implements '\b'
                 * itself as "step one cell left AND blank that cell",
                 * so ONE '\b' here erases the previously echoed
                 * character completely -- the classic print-'b'-space-
                 * backspace triple is not needed with our console.
                 *
                 * GUARD: only erase if the line is not empty.
                 * Erasing with pos == 0 would try to remove a prompt
                 * character: vga_putc clamps at column 0, so nothing
                 * would be corrupted, but the buffer count and the
                 * screen would disagree -- keep them in lockstep: at
                 * least one character has been echoed since the
                 * last newline, so pos > 0 implies the cursor is
                 * strictly right of column 0 and the erase is safe.
                 */
                if (pos > 0) {
                    pos--;
                    line[pos] = '\0';      /* keep the buffer in sync */
                    printk("\b");          /* vga_putc blanks the cell */
                }
                continue;
            }

            if (ch == '\n') {
                /* Enter: command complete. Break out of the
                 * character loop and dispatch.                     */
                break;
            }

            /*
             * Any other printable character: echo it AND store it.
             * THE OVERFLOW GUARD (see the line[] header comment):
             * pos may grow to 127 at most, leaving slot 127 for the
             * NUL. Once full, further keys are echoed but NOT
             * stored -- the user can still hit Enter and the
             * (truncated) command will still dispatch; that is a
             * deliberate, safe failure mode for a debug shell.
             */
            print_char(ch);
            if (pos < 127)
                line[pos++] = ch;
        }

        line[pos] = '\0';   /* terminate (already 0, but be explicit) */

        /* NOTE: no extra '\n' is printed here. keyboard_getchar()
         * already returned the literal '\n' for ENTER and the echo
         * loop handled it in vga_putc -- but vga_putc does NOT store
         * a newline, it only moves the cursor. So we print one here
         * to end the command line visibly... EXCEPT that would
         * double-advance: the '\n' character arrives through the
         * `ch == '\n'` branch which just breaks WITHOUT echoing, so
         * exactly one newline below is correct and not a duplicate. */
        printk("\n");       /* the visible end of the command line   */

        /* ---- Dispatch -------------------------------------------- */
        if (str_eq(line, "")) {
            /* Empty line: no-op, just show the prompt again. A real
             * shell does the same; it costs us nothing.            */
            continue;
        }
        if (str_eq(line, "help")) {
            cmd_help();
        } else if (str_eq(line, "printstack")) {
            cmd_printstack();
        } else if (str_eq(line, "reboot")) {
            cmd_reboot();          /* (does not return)             */
        } else if (str_eq(line, "halt")) {
            cmd_halt();            /* (does not return)             */
        } else {
            /*
             * Unknown: say so, and point at help. Deliberately terse
             * -- a debug shell's error message is not the place for a
             * tutorial. (Note: KFS_2's keyboard driver translates
             * shifted keys to UPPERcase, so "HELP" will not match
             * "help" -- that is a known limitation of the bonus
             * shell, not a bug: it keeps the matcher to a single
             * str_eq and teaches the user exactly what the driver
             * produces.)
             */
            printk("unknown command, try help\n");
        }
    }
}
