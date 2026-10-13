/*
 * include/kernel.h -- the ONE shared header of the KFS_2 kernel.
 * ============================================================================
 *
 * Every .c file includes this header. It is the single source of truth for:
 *   - the public API of each kernel module (printk, GDT, stack dump, shell)
 *   - the GDT hardware layout (packed structs that must match the CPU)
 *   - segment selector constants (what you put in DS/CS/SS after lgdt)
 *   - tiny x86 helpers (outb/inb port I/O, the only "driver" primitives)
 *
 * ----------------------------------------------------------------------------
 * BOOT FLOW -- read this first, it explains who calls what and in which order:
 * ----------------------------------------------------------------------------
 *
 *   BIOS (firmware, 16-bit real mode)
 *     |  boots the first sector of the disk/CD
 *     v
 *   GRUB (bootloader)
 *     |  - parses our multiboot header (declared in boot.asm)
 *     |  - loads kernel.elf into memory at 1 MiB and above
 *     |  - switches the CPU to 32-bit PROTECTED MODE
 *     |    (with GRUB's OWN temporary GDT -- we replace it later)
 *     |  - jumps to our entry symbol `_start` with:
 *          EAX = 0x2BADB002 (multiboot "magic")
 *          EBX = pointer to the multiboot info structure
 *     v
 *   boot.asm:_start        (assembly: set up the stack, save registers)
 *     |  mov esp, stack_top ; push ebx ; push eax ; call kmain
 *     v
 *   kmain()  (src/kmain.c) -- our first C code
 *       1. printk()          -- so we can SEE anything at all (src/printk.c)
 *       2. gdt_install()     -- our real GDT at 0x800 (src/gdt.c)
 *                             + gdt_flush (src/gdt_flush.asm, lgdt + reload)
 *       3. stack_dump()      -- human-readable stack dump (src/stack_dump.c)
 *       4. shell_run()       -- bonus debug shell (src/shell.c)
 *
 * ----------------------------------------------------------------------------
 * RULES OF THIS KERNEL (why the code looks the way it does):
 * ----------------------------------------------------------------------------
 *   - TARGET: bare-metal i386 (32-bit x86). Nothing from the host OS (macOS)
 *     exists at runtime: no libc, no malloc, no files, no syscalls.
 *   - COMPILE: freestanding (-ffreestanding -nostdlib ...). The only headers
 *     we may include are the freestanding ones the compiler itself provides:
 *     <stdint.h>, <stddef.h>, <stdarg.h>, <stdbool.h>. That is what we use.
 *   - EVERYTHING is built from first principles: the screen is memory at a
 *     fixed address, the keyboard is a byte read from a port, the GDT is a
 *     table at a fixed address the CPU is told about via one instruction.
 *
 * Further reading (also gathered in README.md):
 *   - OSDev wiki "Bare Bones"           : the minimal boot skeleton
 *   - OSDev wiki "Meaty Skeleton"       : how to structure a freestanding kernel
 *   - OSDev wiki "GDT Tutorial"         : the table this project centers on
 *   - Intel SDM Vol. 3, ch. 3 & 5       : segmentation, the authoritative text
 * ============================================================================
 */

#ifndef KFS_KERNEL_H
#define KFS_KERNEL_H

/* ---------------------------------------------------------------------------
 * The only host-free headers a freestanding C compiler must provide.
 * stdint.h -> fixed-width types (uint8_t, uint32_t, ...): MANDATORY for
 *             hardware structs, where every bit and byte counts.
 * stddef.h -> size_t, NULL.
 * stdarg.h -> va_list etc., needed by printk's implementation.
 * ------------------------------------------------------------------------ */
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* ===========================================================================
 * 1. MULTIBOOT
 * ===========================================================================
 * We use the "multiboot" boot protocol: a standard that says "if the kernel
 * embeds this magic header in its first 8 KiB, any compliant bootloader (GRUB)
 * can load it, switch to protected mode, and hand control over with EAX and
 * EBX set as follows".
 */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002u
/* EAX at entry: proof that a multiboot-compliant bootloader started us.     */
/* EBX at entry: physical address of the "multiboot info" struct (a summary
 * of memory map, boot device, ...). We only print it this iteration.        */

/* ===========================================================================
 * 2. PORT I/O -- how the CPU talks to devices that are not memory
 * ===========================================================================
 *
 * x86 has TWO address spaces:
 *   - memory (what pointers reach: VGA text is there, at 0xB8000)
 *   - I/O ports (what the IN/OUT instructions reach: keyboard, serial UART,
 *     programmable interrupt controller, ...)
 *
 * outb(value, port): writes one byte to an I/O port.
 * inb(port)        : reads one byte from an I/O port.
 *
 * The inline assembly below means: emit the `outb %al, %dx` instruction with
 * AL = val and DX = port. "a" and "Nd" are GCC constraint letters: they tell
 * the compiler to put `val` in register AL, and `port` in DX allowing the
 * one-byte immediate form. `volatile` forbids the compiler from reordering
 * or deleting these writes -- device communication has side effects, it is
 * NOT like computing a value.
 */
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* A write to port 0x80 takes ~1us and does nothing; devices behind a slow
 * bus (like the keyboard controller) sometimes need a tiny pause between
 * accesses. Classic trick: "junk write" used as a delay.                    */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/* ===========================================================================
 * 3. GDT -- Global Descriptor Table (the heart of KFS_2)
 * ===========================================================================
 *
 * In protected mode, the CPU does not use raw pointers-to-memory for
 * segmentation: every memory access goes through a SEGMENT DESCRIPTOR, an
 * 8-byte record that says "this segment starts at BASE, is LIMIT bytes long,
 * and code/data may access it with these PRIVILEGES".
 *
 * All descriptors live in one table -- the GDT. The CPU finds it through the
 * GDTR register, which we load with one LGDT instruction (src/gdt_flush.asm).
 * A segment register (CS, DS, SS, ES, FS, GS) then holds not an address but
 * a 16-bit SELECTOR: an index into that table.
 *
 * KFS_2 SPEC REQUIREMENT: our GDT must live at physical address 0x00000800
 * and contain EXACTLY these 6 segments (plus the mandatory null entry):
 *
 *   index 0 : null descriptor          (required by the CPU, never usable)
 *   index 1 : kernel code   -> selector 0x08
 *   index 2 : kernel data   -> selector 0x10
 *   index 3 : kernel stack  -> selector 0x18
 *   index 4 : user code     -> selector 0x23   (note: RPL 3 -- see below)
 *   index 5 : user data     -> selector 0x2B
 *   index 6 : user stack    -> selector 0x33
 *
 * WHY AT 0x800, AND WHY WE WRITE IT AT RUNTIME:
 * GRUB loads our kernel image at 1 MiB or above (multiboot rule), so the GDT
 * CANNOT be a normal variable in our .data section -- it would land above
 * 1 MiB, never at 0x800. Instead gdt_install() (src/gdt.c) BUILDS the table
 * byte by byte at physical address 0x800, inside the low "conventional
 * memory" area the BIOS owns before the kernel is loaded.
 *
 * Selector anatomy:  selector = (index << 3) | RPL
 *   - index      : which descriptor (index * 8 = byte offset in the table)
 *   - RPL bits0-1: "requested privilege level", 0 = kernel, 3 = user.
 *     User selectors therefore have | 3 (e.g. 0x20 | 3 = 0x23).
 */
#define GDT_ADDR     0x00000800u  /* physical address mandated by the spec   */
#define GDT_ENTRIES  7            /* null + 6 segments                       */

/* One 8-byte segment descriptor, EXACTLY as the CPU reads it (Intel SDM
 * Vol. 3, figure 3-8 "Segment Descriptor"). The field order looks bizarre
 * because Intel reused bits across decades of history -- that is WHY this
 * struct must be __attribute__((packed)) and byte-exact:
 *
 *   byte 0-1 : LIMIT  [15:0]  (low half of the 20-bit limit)
 *   byte 2-4 : BASE   [23:0]  (low 3 bytes of the 32-bit base)
 *   byte 5   : ACCESS byte     (type, privilege, present -- table below)
 *   byte 6   : FLAGS(4 bits) | LIMIT[19:16]
 *   byte 7   : BASE   [31:24]
 *
 * ACCESS byte, bit by bit:
 *   bit 7    P   : present (1 = segment is valid). 0 for the null descriptor.
 *   bits 6-5 DPL  : descriptor privilege level: 0 = kernel, 3 = user.
 *   bit 4    S   : 1 = code/data segment, 0 = system segment (TSS, gates...).
 *   bit 3    E   : executable (1 = code, 0 = data).
 *   bit 2    DC  : direction/conforming (for stacks/data: 0 = grows up).
 *   bit 1    RW  : readable (code) / writable (data). A stack NEEDS this = 1.
 *   bit 0    A   : accessed (CPU sets it; we always clear it).
 *
 * So the classic access bytes:
 *   0x9A = 1001 1010 = P|DPL0|S|code|readable  -> kernel code
 *   0x92 = 1001 0010 = P|DPL0|S|data|writable  -> kernel data & stack
 *   0xFA = 1111 1010 = P|DPL3|S|code|readable  -> user code
 *   0xF2 = 1111 0010 = P|DPL3|S|data|writable  -> user data & stack
 *
 * FLAGS nibble (high 4 bits of byte 6):
 *   bit 3  G  : granularity: 0 = limit is in bytes, 1 = limit is in 4 KiB
 *              pages. We use 1 so limit 0xFFFFF means the full 4 GiB.
 *   bit 2  D/B: 1 = 32-bit segment (default operand/address size). i386!
 *   bit 1  L  : 64-bit only, must be 0.
 *   bit 0  AVL: "available" for the OS, unused, 0.
 *   So flags byte 0xC0 = G=1, D/B=1.
 */
struct gdt_entry {
    uint16_t limit_low;         /* limit bits 15..0                          */
    uint16_t base_low;          /* base bits 15..0                           */
    uint8_t  base_mid;          /* base bits 23..16                          */
    uint8_t  access;            /* P DPL S E DC RW A (table above)           */
    uint8_t  flags_limit_high;  /* G D/B L AVL | limit bits 19..16           */
    uint8_t  base_high;         /* base bits 31..24                          */
} __attribute__((packed));      /* packed: no padding, or the CPU misreads it */

/* GDTR is loaded with a 6-byte record: a 16-bit LIMIT (table size - 1, in
 * bytes) and a 32-bit linear BASE ADDRESS (the address of the table itself,
 * i.e. 0x800 here). Passing the table instead of this record to lgdt is a
 * classic triple-fault bug.                                             */
struct gdt_ptr {
    uint16_t limit;             /* size of the GDT in bytes, minus one       */
    uint32_t base;              /* linear address of the first descriptor    */
} __attribute__((packed));

/* Selector constants (see "Selector anatomy" above). */
#define SEL_KCODE  0x08    /* (1 << 3) | 0  */
#define SEL_KDATA  0x10    /* (2 << 3) | 0  */
#define SEL_KSTACK 0x18    /* (3 << 3) | 0  */
#define SEL_UCODE  0x23    /* (4 << 3) | 3  */
#define SEL_UDATA  0x2B    /* (5 << 3) | 3  */
#define SEL_USTACK 0x33    /* (6 << 3) | 3  */

/* gdt_install(): fill the 7 descriptors at 0x800, set up struct gdt_ptr,
 * print a summary via printk, then call gdt_flush() below.                */
void gdt_install(void);

/* gdt_flush(gdtr_addr): ASSEMBLY (src/gdt_flush.asm). Takes the ADDRESS of a
 * struct gdt_ptr, executes `lgdt [gdtr_addr]`, reloads DS/ES/FS/GS with
 * SEL_KDATA, SS with SEL_KSTACK, and far-jumps to SEL_KCODE -- a far jump
 * is the only way to reload CS from a selector.
 * Implemented in assembly because C cannot express lgdt or a far jump.    */
void gdt_flush(uint32_t gdtr_addr);

/* ===========================================================================
 * 4. THE KERNEL STACK
 * ===========================================================================
 *
 * The x86 hardware stack GROWS DOWNWARD: PUSH decrements ESP, POP increments
 * it. So a reserved stack area looks like this in memory:
 *
 *      low addresses
 *        ...
 *        esp ->  last pushed value      \   used part
 *                ...                    |   ("used" = [esp, stack_top))
 *        stack_bottom  (lowest address) /
 *        (16 KiB of reserved .bss)
 *        ...
 *        stack_top  (highest address) -- where _start points ESP at the start
 *
 * boot.asm reserves 16 KiB in .bss, 16-byte aligned, and exports the two
 * boundary symbols below (they are ADDRESSES, not variables -- extern arrays).
 * KERNEL_STACK_SIZE must stay in sync with boot.asm's `resb 16384`.
 */
#define KERNEL_STACK_SIZE 16384u

extern uint8_t stack_bottom[];  /* lowest address of the stack area          */
extern uint8_t stack_top[];     /* one PAST the highest usable address       */

/* ===========================================================================
 * 5. CONSOLE: printk
 * ===========================================================================
 *
 * Before any output exists, the kernel is invisible. printk is the kernel's
 * printf: it writes text to the VGA text buffer (80x25 characters at physical
 * address 0xB8000) AND mirrors every byte to the COM1 serial port, so the
 * same output can be captured headlessly with:
 *     qemu-system-i386 -kernel kernel.elf -display none -serial stdio
 *
 * Supported format specifiers (kept deliberately small):
 *   %c  one character          %s  NUL-terminated string
 *   %x  lowercase hex, no padding      %d  signed decimal
 *   %u  unsigned decimal       %%  a literal '%'
 * Raw control characters passed through the format string are also
 * handled: '\n' (newline + wrap/scroll), '\b' (erase one cell), and
 * '\r' (return to column 0).
 * For zero-padded fixed-width hex (addresses, dumps) use printk_hex below.
 */
void printk(const char *fmt, ...);

/* Print `value` as lowercase hex, zero-padded to `width` digits (1..8).
 * Example: printk_hex(0x800, 8) prints "00000800".                         */
void printk_hex(uint32_t value, int width);

/* ===========================================================================
 * 6. STACK DUMP (the mandatory "print the kernel stack, human-friendly" tool)
 * ===========================================================================
 * Reads ESP/EBP at call time, then prints the used part of the kernel stack
 * ([esp, stack_top)) as a classic hexdump:
 *
 *   0x00107FF0  ffffffff 00000000 ...  |....|
 *
 * with one address column, 4 hex words per row (16 bytes), an ASCII
* column, and the
 * ESP/EBP positions marked. Implementation: src/stack_dump.c.
 */
void stack_dump(void);

/* ===========================================================================
 * 7. DEBUG SHELL (bonus)
 * ===========================================================================
* A minimal blocking shell: prints "kfs2> ", reads a line from the PS/2
* keyboard (polled, no interrupts needed), and dispatches commands:
*   help | printstack | reboot | halt
* Implementation: src/shell.c (+ src/keyboard.c for scancodes).
*/
void shell_run(void);

/* keyboard_getchar(): blocking read of ONE translated character from the
* PS/2 keyboard (scancode set 1, polled -- no IDT/PIC involved).
* Returns the character (0x20..0x7E, or '\n' / '\b'), or -1 when the
* scancode should be ignored (key releases, unknown codes, bare shift
* presses). Implemented in src/keyboard.c; used by src/shell.c.       */
int keyboard_getchar(void);

/* ===========================================================================
 * 8. KERNEL ENTRY POINT
 * ===========================================================================
 * Called by boot.asm. Arguments (cdecl, pushed right-to-left by _start):
 *   magic    = EAX: MULTIBOOT_BOOTLOADER_MAGIC if GRUB loaded us
 *   mbi_addr = EBX: physical address of the multiboot info struct
 * This function never returns (it ends inside the shell's halt command).
 */
void kmain(uint32_t magic, uint32_t mbi_addr);

#endif /* KFS_KERNEL_H */