/*
 * src/kmain.c -- the kernel's C entry point.
 * ============================================================================
 *
 * Everything before this file is plumbing: BIOS -> GRUB -> boot.asm. kmain()
 * is the first line of OUR code, and the conductor of the whole project:
 *
 *   kmain
 *    |-- printk(...)        : console works, we can finally SEE something
 *    |-- gdt_install()      : our own GDT at 0x800 replaces GRUB's temp one
 *    |-- stack_dump()       : mandatory "print the kernel stack" tool
 *    `-- shell_run()        : bonus debug shell (never returns)
 *
 * ----------------------------------------------------------------------------
 * WHAT STATE IS THE CPU IN WHEN WE GET HERE? (worth memorizing)
 * ----------------------------------------------------------------------------
 *   - 32-bit protected mode, thanks to GRUB (it did the real-mode switch).
 *   - DS/ES/FS/GS/SS and CS point through GRUB's OWN temporary GDT. That GDT
 *     is flat (base 0, limit 4 GiB) -- so plain pointers just work -- but it
 *     is NOT ours and must be replaced by gdt_install() before we can claim
 *     to have "declared our GDT to the BIOS" as the subject demands.
 *   - Interrupts are disabled: boot.asm executed `cli` first thing. We keep
 *     them off (we poll the keyboard instead) so no IDT is needed yet.
 *   - ESP points to the top of OUR 16 KiB stack, set by boot.asm one
 *     instruction before the call -- C needs a stack for locals/returns, so
 *     this had to happen before kmain could ever run.
 *   - EAX/EBX held the multiboot magic and info pointer; boot.asm pushed them
 *     so they arrive here as arguments (cdecl order: last arg pushed first).
 *
 * ----------------------------------------------------------------------------
 * WHY printk FIRST, AND WHY IT WORKS WITHOUT OUR GDT
 * ----------------------------------------------------------------------------
 * Writing to the VGA text buffer is just writing to physical address
 * 0xB8000. With a flat GDT (base 0, limit 4 GiB) any physical address below
 * 4 GiB is reachable with a plain pointer. GRUB's flat GDT already allows
 * that, so the screen works BEFORE we install our own table. It is the
 * cheapest possible sanity signal: if this line does not print, the problem
 * is in the boot chain, not in the GDT code.
 *
 * Further reading: OSDev wiki "Bare Bones" (this boot sequence), "Meaty
 * Skeleton" (kernel code organization).
 * ============================================================================
 */

#include "../include/kernel.h"

void kmain(uint32_t magic, uint32_t mbi_addr)
{
    /*
     * First output ever. If you see this line (on the VGA screen in the QEMU
     * window, or via `-serial stdio`), the whole chain worked:
     * GRUB found the multiboot header, loaded the ELF, set up protected mode,
     * boot.asm set the stack, and the linker placed everything correctly.
     */
    printk("KFS_2 booting: multiboot magic=%x info=%x\n", magic, mbi_addr);

    /*
     * Defensive check: GRUB only jumps here with EAX = 0x2BADB002. If it is
     * different, something ELSE started us (not a multiboot bootloader) and
     * EBX is garbage -- better to say so loudly than to trust it.
     */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        printk("WARNING: unexpected boot magic %x, multiboot info untrusted\n",
               magic);

    /*
     * The actual subject: build the 7-descriptor GDT at physical 0x800 and
     * load it with lgdt (see src/gdt.c and src/gdt_flush.asm for the full
     * byte-level story of access bytes, flags, selectors and far jumps).
     * After this call, CS = SEL_KCODE, SS = SEL_KSTACK, ... all through OUR
     * table -- and it prints a summary of what it installed.
     */
    gdt_install();

    /*
     * Mandatory part: dump the used part of the kernel stack (from current
     * ESP up to stack_top) as an address / hex / ASCII table with the ESP and
     * EBP positions marked. At boot the used part is tiny -- only the return
     * address that `call kmain` pushed -- which is exactly the point: it
     * makes the stack mechanics visible.
     */
    stack_dump();

    /*
     * Bonus part: interactive debug shell. It polls the PS/2 keyboard (no
     * interrupts, so no IDT/PIC are needed) and offers printstack / reboot /
     * halt. This call never returns: without a scheduler, a shell that has
     * nothing left to do simply loops forever reading the keyboard.
     */
    shell_run();
}