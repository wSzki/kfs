/* ============================================================================
 * linker.ld -- the KFS_2 GNU ld script.
 * ============================================================================
 * ROLE
 * ----
 * The linker script is the map of the kernel image: it tells i686-elf-ld
 * where every section goes, what order they appear in, and what the final
 * ELF must export. Without one, we would be at the mercy of the cross
 * linker's built-in default script -- which assumes a user-space process
 * (crt0, libc, brk() heap, program headers for the host ABI). A kernel is
 * NOT a process, so we must layout ourselves.
 *
 * INVARIANTS THIS SCRIPT ENFORCES (from AGENTS.md / the subject):
 *   - Everything sits at 1 MiB and above: GRUB (multiboot) loads the
 *     kernel there, and our own GDT lives at 0x800, BELOW the image.
 *   - .multiboot is emitted as the FIRST bytes of the image (see the
 *     8 KiB rule explained under .text below).
 *   - kernel_start / kernel_end symbols are exported: the loader side
 *     (and anything that wants to iterate the image) can see the exact
 *     span of the kernel, e.g. to copy it or zero its .bss.
 *
 * FURTHER READING
 *   - OSDev "Linker Scripts":  https://wiki.osdev.org/Linker_Scripts
 *   - OSDev "ELF":            https://wiki.osdev.org/ELF
 *   - GNU ld manual, "Linker Script Inputs" (the section wildcards)
 * ============================================================================ */

/* OUTPUT_FORMAT: emit a 32-bit i386 ELF. The cross i686-elf-ld already
 * defaults to this, but writing it out makes the target of every .o and
 * the final kernel.elf explicit and greppable -- and it fails loudly if
 * somebody ever runs the wrong ld. */
OUTPUT_FORMAT("elf32-i386")

/* OUTPUT_ARCH: the CPU class this image targets. i386 (486-class
 * instructions and up, but the C code is compiled with -march=i386, so
 * in practice nothing newer than i386 is ever emitted). */
OUTPUT_ARCH(i386)

/* ENTRY(_start): the entry point of the final ELF. GRUB reads the ELF
 * e_entry field and jumps THERE -- so _start (our boot stub in
 * boot.asm) must be it, not some linker-invented start. This is also
 * why boot.asm declares `global _start`. */
ENTRY(_start)

SECTIONS
{
    /* ---------------------------------------------------------------------
     * WHERE THE IMAGE LIVES, AND WHY SO HIGH
     * ---------------------------------------------------------------------
     * The first 1 MiB of i386 memory is a minefield of firmware- and
     * hardware-reserved regions. The classic "low memory map" every x86
     * student should know by heart:
     *
     *   0x00000 - 0x003FF   Interrupt Vector Table (IVT): 256 real-mode
     *                       vectors x 4 bytes = 1 KiB, written by the BIOS.
     *   0x00400 - 0x004FF   BIOS data area (BDA). The POST code writes
     *                       video mode, disk counts, memory size here.
     *   0x00500 - 0x07BFF   free conventional RAM (BIOS scratch ends here).
     *   0x00800 - 0x00BFF   where OUR GDT goes (GDT_ADDR = 0x800 in
     *                       include/kernel.h): 7 descriptors x 8 bytes,
     *                       comfortably inside the free area above.
     *   0x07C00 - 0x07DFF   the MBR: the first 512 bytes of the boot disk,
     *                       copied there by the BIOS when it boots.
     *   0x07E00 - 0x9FBFF   free conventional RAM.
     *   0x9FC00 - 0x9FFFF   Extended BIOS Data Area (EBDA) -- size varies.
     *   0xA0000 - 0xBFFFF   video memory owned by the VGA card:
     *                       0xA0000 = graphics frame buffer, and
     *                       0xB8000 = the TEXT buffer printk writes to
     *                       (80x25 cells x 2 bytes = 4000 bytes = 0xFA0).
     *   0xC0000 - 0xEFFFF   option ROMs (video BIOS, NIC boot ROMs).
     *   0xF0000 - 0xFFFFF   the system BIOS ROM itself (last 64 KiB).
     *   0x100000 and up     conventional RAM for the kernel: where GRUB
     *                       loads US, per the multiboot standard.
     *
     * CONSEQUENCE: we never write anywhere below 1 MiB ourselves
     * except the one mandated exception, the GDT at 0x800, which is
     * exactly why the GDT's address is specified in the subject rather
     * than left to us. The kernel image as a whole is placed at 1 MiB
     * (`1M` == 0x100000), which is where multiboot/GRUB loads it, and
     * which keeps the image clear of every region above. */
    . = 1M;

    /* .text: the executable code of the kernel.
     *
     * KEEP(*(.multiboot)) FIRST: the multiboot header section, emitted
     * verbatim (KEEP = "do not garbage-collect even though nothing
     * references it"), before ANY other code. The multiboot spec says
     * the header must be within the first 8 KiB of the binary; emitting
     * it as byte 0 of the image guarantees that forever, no matter what
     * code we add. If it were placed later (or discarded), GRUB's 8 KiB
     * scan would miss the magic and refuse to load the kernel with
     * "no multiboot header found" -- a boot failure that is 100% silent
     * to the kernel code itself.
     *
     * The trailing ALIGN(4K) rounds the end of .text up to a page
     * boundary before .rodata starts. Every section in a kernel image
     * should start on a 4 KiB boundary: it matches the ELF "segment
     * alignment" that GRUB relies on when it loads the program headers,
     * and it keeps each section's virtual address equal to a clean
     * multiple of the page size -- which is what makes "linear ==
     * physical" trivially true under our flat GDT. */
    .text : {
        /* KEEP(*(.multiboot)) FIRST: the multiboot header, emitted verbatim
         * (KEEP = "do not garbage-collect even though nothing references
         * it"), before ANY other code. The multiboot spec requires the
         * header within the first 8 KiB of the binary; emitting it as byte
         * 0 of the image guarantees that forever. If it were placed later
         * or discarded, GRUB's scan would miss the magic and refuse to
         * load the kernel with "no multiboot header found" -- a boot
         * failure that is 100% silent to the kernel code itself. */
        KEEP(*(.multiboot))
        /* then all the actual code. The `.*` wildcard catches
         * compiler-renamed sub-sections (.text.startup, .text.unlikely,
         * ...) so none of the code is dropped. NOTE: GNU ld comments are
         * C-style block comments ONLY -- there is no semicolon comment
         * in linker scripts; a stray semicolon is a syntax error at
         * link time. */
        *(.text .text.*)
    }

    /* 4 KiB padding between sections: every section starts on a page
     * boundary. This matches the ELF segment alignment GRUB relies on
     * when loading program headers, and keeps "linear == physical"
     * trivially true under our flat GDT. */
    . = ALIGN(4K);

    /* .rodata: read-only data -- string literals ("KFS_2 booting: ..."),
     * constant arrays, format tables. Read-only so the CPU can keep it
     * in read-only pages; putting it AFTER .text also means the GDT at
     * 0x800 and the kernel image never overlap. */
    .rodata : {
        *(.rodata .rodata.*)
    }
    . = ALIGN(4K);

    /* .data: initialized, writable data (globals with initializers).
     * (This week the kernel has almost none; the section exists so that
     * the moment we add one, the layout rule is already in place.) */
    .data : {
        *(.data .data.*)
        /* COMMON blocks: zero-initialized globals older C code merged
         * as "common symbols". Without this line ld would place them
         * somewhere arbitrary or drop them. */
        *(COMMON)
    }
    . = ALIGN(4K);

    /* .bss: uninitialized data -- this is where OUR KERNEL STACK LIVES
     * (stack_bottom..stack_top in boot.asm, 16 KiB). The ELF records
     * .bss as "no file bytes, this many bytes to reserve": the bootloader
     * must allocate and zero it before jumping to us, which GRUB does.
     * ALIGN(4K) after the section pads the END of the image to a page
     * boundary -- the next thing a future version of this kernel may do
     * is start a page allocator, and page-aligned image ends are what
     * page allocators need to find the "first free page". */
    .bss : {
        *(.bss .bss.*)
    }
    /* Page-align the END of the image: the next thing a future version
     * of this kernel may do is start a page allocator, and page-aligned
     * image ends are what page allocators need to find the "first free
     * page". */
    . = ALIGN(4K);

    /* The image ends here. Export the two symbols the loader side (and
     * this project's tooling) may want:
     *
     *   kernel_start = the very first byte of the kernel image
     *                  (== 0x100000, because `. = 1M` and .text is the
     *                   first section).
     *   kernel_end   = the first byte AFTER the last section, after all
     *                  4K padding above.
     *
     * Both are `PROVIDE`-style assignments: plain symbol definitions in
     * the output; any tool that links against the kernel, or any future
     * loader code that wants to "find the end of the kernel", can just
     * reference them by name. (PROVIDE() would mean "only if nobody
     * defines them"; we WANT to define them, so plain `=`.) */
    kernel_start = 1M;
    kernel_end   = .;

    /* /DISCARD/: sections that user-space tooling adds to every ELF but
     * that a kernel must NOT ship.
     *
     *   .comment: the "GCC: (i686-elf-gcc) 13.2"-style toolchain banner
     *             the compiler appends. Harmless but pure noise in a
     *             hand-built kernel; more importantly, keeping it forces
     *             the linker to emit an extra section whose layout we
     *             did not ask for.
     *   .eh_frame: the C/C++ exception & unwind table. A freestanding
     *               kernel has no exception runtime, no stack unwinder,
     *               and no one to read .eh_frame; shipping it is image
     *               bloat that would also have to be relocated if the
     *               image moved. Worse: a .eh_frame section in the ELF
     *               that the loader does not know how to handle is a
     *               "section has no data" or "relocation out of range"
     *               failure class at link/boot time. -fno-asynchronous-
     *               unwind-tables in CFLAGS stops the compiler from
     *               generating most of it, and /DISCARD/ is the safety
     *               net for whatever still leaks out (some headers and
     *               some flags force one on regardless).
     *
     * /DISCARD/ takes the sections and throws them away entirely: they
     * consume no address space and do not appear in the final ELF at
     * all. */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame .eh_frame.*)
        *(.eh_frame_hdr)
    }
}
