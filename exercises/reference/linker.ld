/* linker.ld — tells the linker WHERE each piece goes in memory.
   Without this, we'd get the host OS's default layout: wrong for a kernel. */

ENTRY(_start)              /* kernel's first instruction = _start from boot.asm */

SECTIONS
{
    /* Place the kernel at 1 MiB.
       Below 1 MiB lives legacy territory (VGA at 0xB8000, BIOS data).
       1 MiB is the classic "safe, free" load address for kernels. */
    . = 1M;

    .multiboot : {          /* header FIRST → stays within the first 8 KB */
        *(.multiboot)
    }

    .text : {               /* executable code */
        *(.text)
    }

    .bss : {                /* uninitialized data — our stack lives here */
        *(.bss)
    }
}
