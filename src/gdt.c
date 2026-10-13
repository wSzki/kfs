/*
 * src/gdt.c -- our Global Descriptor Table: the kernel's own map of memory.
 * ============================================================================
 *
 * ROLE IN THE BOOT CHAIN
 * ----------------------------------------------------------------------------
 * kmain() calls gdt_install() the moment the console works. Before this
 * call, every segment register in the CPU points into GRUB's OWN temporary
 * GDT (GRUB built one at 1 MiB+ just to get protected mode running). That
 * table is not ours: we do not know its layout, we cannot extend it, and
 * the assignment spec is explicit -- the kernel must install ITS OWN GDT at
 * physical address 0x00000800 with exactly six named segments plus the
 * mandatory null descriptor. After gdt_install() returns, CS, DS, ES, FS,
 * GS and SS all point through OUR table, and the GDT summary printed here
 * is the "proof of work" the grader expects to see on screen.
 *
 * The call order inside this file:
 *
 *   kmain()
 *     |-> gdt_install()
 *          1. set_gate() x 7   : write the 56 bytes of the table at 0x800
 *          2. build struct gdt_ptr (the 6-byte GDTR record)
 *          3. print a summary of the table (via printk + printk_hex)
 *          4. gdt_flush()      : assembly (src/gdt_flush.asm) -- the actual
 *                                 lgdt + segment-register reload + far jump.
 *                                 C cannot write CS or execute lgdt, so the
 *                                 last step crosses into assembly.
 *
 * CONCEPTS THIS FILE INTRODUCES
 * ----------------------------------------------------------------------------
 *   1. Why a hardware-mandatory table CANNOT be a normal C variable: GRUB
 *      loads the kernel at >= 1 MiB, the spec demands physical 0x800, so we
 *      reach backwards into low memory through a fixed-address pointer.
 *   2. The 8-byte descriptor layout and how a 20-bit limit and a 32-bit
 *      base are SPLIT across the bytes (set_gate, bit ranges below).
 *   3. The flat model: base 0, limit 4 GiB -- "segmentation exists, but is
 *      invisible", and privilege separation lives in DPL/RPL, not in bases.
 *   4. _Static_assert: catching struct layout drift AT COMPILE TIME instead
 *      of at the moment the CPU triple-faults at boot.
 *   5. The 6-byte GDTR record (limit + base) and why lgdt wants that
 *      record's ADDRESS, not the table itself (the classic triple-fault).
 *
 * FURTHER READING (read in this order)
 * ----------------------------------------------------------------------------
 *   - OSDev wiki "GDT Tutorial": https://wiki.osdev.org/GDT_Tutorial
 *       (the exact table layout, the access-byte table, and the lgdt
 *        sequence this file implements, with screenshots)
 *   - Intel SDM Vol. 3, ch. 3.5 "Segmentation" and ch. 5.6 "Using
 *       Segments" -- the authoritative bit-level text for everything here.
 *   - The header comment in include/kernel.h (section 3) already unpacks
 *     the descriptor byte layout and the selector arithmetic; this file
 *     assumes you have read it.
 * ============================================================================
 */

#include "../include/kernel.h"

/* ===========================================================================
 * WHERE THE TABLE LIVES, AND WHY IT CANNOT BE A VARIABLE
 * ===========================================================================
 *
 * A GDT is not "data we happen to keep"; it is hardware state. The CPU
 * fetches these bytes through GDTR every time it loads a segment register,
 * and the assignment spec fixes the location: physical address 0x800.
 *
 * We CANNOT declare the table as a normal array:
 *
 *     struct gdt_entry gdt[GDT_ENTRIES];   /* DON'T: would land at 1 MiB+ */
 *
 * because of the multiboot loading rule: GRUB loads the kernel image (its
 * .text/.data/.bss) at physical address 1 MiB or ABOVE. Any C variable
 * lives inside that image, so it would sit above 1 MiB -- and the CPU is
 * told "your GDT is at 0x800", which is a DIFFERENT 56 bytes, far below
 * the kernel image, inside the low conventional memory region (the first
 * 1 MiB of physical RAM) that the BIOS/GRUB era owns.
 *
 * So the kernel writes the table there AT RUNTIME, reaching backwards into
 * low memory through a pointer constant:
 *
 *     0x0000            0x00000800          0x00001000              1 MiB
 *     +---------------+ ... +--------------+ ... +----------------------+
 *     | BIOS data     |     |  OUR GDT     |     |  kernel image        |
 *     | area          |     |  (56 bytes)  |     |  (GRUB's load zone,  |
 *     |               |     |              |     |  kmain runs here)    |
 *     +---------------+ ... +--------------+ ... +----------------------+
 *
 * The `const` on the pointer says the POINTER never moves; the bytes it
 * points to are freely written below. A cast is needed because 0x800 is a
 * plain integer and we need the struct type to reach field-by-field.
 */
static struct gdt_entry *const gdt = (struct gdt_entry *)GDT_ADDR;

/* ===========================================================================
 * set_gate(): write one 8-byte descriptor into the table
 * ===========================================================================
 *
 * A descriptor looks like one contiguous 8-byte blob when you hand it to
 * the CPU, but the struct in kernel.h stores that blob as SIX separate
 * fields, so every write must SPLIT a 20-bit limit and a 32-bit base
 * across non-contiguous fields. This helper does that once, and keeps the
 * call sites readable.
 *
 * The bit surgery, field by field (Intel SDM Vol. 3, figure 3-8):
 *
 *   limit is 20 bits wide (we address up to 2^20 * 4 KiB = 4 GiB):
 *     - bits 15..0  -> limit_low         (low 16, one whole field)
 *     - bits 19..16 -> high nibble of the flags_limit_high field
 *                      (only its LOW 4 bits -- the top 4 bits of that
 *                      byte hold the G/D flags, which we pass in whole)
 *
 *   base is 32 bits wide, stored in THREE places, split into 16/8/8:
 *     - bits 15..0  -> base_low          (one uint16 field)
 *     - bits 23..16 -> base_mid          (one byte)
 *     - bits 31..24 -> base_high         (one byte)
 *
 *   access  (byte 5)  -> written verbatim, the bit table is in kernel.h.
 *   flags   (byte 6)  -> flags_limit_high = flags (high nibble) | limit[19:16]
 *
 * Every shift/mask lands in a narrower type with an explicit cast: the
 * values are small, but we cast so that nothing is implementation-defined.
 */
static void set_gate(uint32_t index,
                     uint32_t base,
                     uint32_t limit,
                     uint8_t access,
                     uint8_t flags_limit_high)
{
    gdt[index].limit_low = (uint16_t)(limit & 0x0000FFFFu);
    gdt[index].base_low  = (uint16_t)(base & 0x0000FFFFu);
    gdt[index].base_mid  = (uint8_t)((base >> 16) & 0x000000FFu);
    gdt[index].access    = access;
    gdt[index].flags_limit_high =
        (uint8_t)((flags_limit_high & 0x000000F0u) | ((limit >> 16) & 0x0000000Fu));
    gdt[index].base_high = (uint8_t)((base >> 24) & 0x000000FFu);
}

/* ===========================================================================
 * THE LAYOUT TRAP, CAUGHT AT COMPILE TIME
 * ===========================================================================
 *
 * The CPU reads these bytes in hardware; C only gives us a type system.
 * `_Static_assert` (C11, supported by the compiler even in freestanding
 * mode) makes the compiler PROVE the struct is exactly as wide as the
 * hardware record, or refuse to build:
 *
 *   - gdt_entry: six fields summing to 16+16+8+8+8+8 bits = 8 bytes. If
 *     someone drops the `__attribute__((packed))` off kernel.h, or reorders
 *     fields and the compiler sneaks in padding, the struct grows, every
 *     descriptor shifts by whole bytes, the CPU decodes garbage, and the
 *     boot triple-faults with zero diagnostics. The assert turns that
 *     into a clear compile error -- months before anyone would have hit
 *     the fault screen.
 *
 *   - gdt_ptr: a 16-bit limit followed by a 32-bit base is 10 bytes with
 *     natural alignment (2 padding bytes after the uint16_t), but LGDT
 *     reads exactly 6. Losing packed here means lgdt fetches a wrong base
 *     and the far jump dies -- so this assert is the real one: without
 *     packing, sizeof(gdt_ptr) != 6 and the build stops.
 */
_Static_assert(sizeof(struct gdt_entry) == 8,
               "gdt_entry must be exactly 8 bytes (packed), matching the CPU");
_Static_assert(sizeof(struct gdt_ptr) == 6,
               "gdt_ptr must be exactly 6 bytes (packed); LGDT reads 6");

/* ===========================================================================
 * gdt_install(): fill the 7 descriptors, build the GDTR record, print, flush
 * ===========================================================================
 *
 * THE FLAT MODEL -- why every descriptor below is identical except for
 * the access byte:
 *
 *   base   = 0        : the segment starts at physical address zero.
 *   limit  = 0xFFFFF  : with the GRANULARITY bit set, the limit is counted
 *                       in 4 KiB pages, so 0xFFFFF pages = 4 GiB - 1 byte:
 *                       the whole 32-bit address space.
 *
 * Segmentation therefore "exists but is invisible": linear address ==
 * physical address for everything below 4 GiB, which is why plain C
 * pointers keep working after we flush. What segmentation still does for
 * us is PRIVILEGE: the DPL field in the access byte (bits 6-5) plus the
 * RPL bits in the selector are checked on every CS/DS/SS use. That is the
 * whole point of having DPL-3 user descriptors at all even though their
 * bases are flat. (When this kernel later gains processes, these same
 * flat segments are what ring 3 will run in.)
 *
 * THE SEVEN ENTRIES (fixed order demanded by the spec -- index * 8 + RPL
 * gives each selector, see the SEL_* constants in kernel.h):
 *
 *   0  null      : all zero. The CPU REQUIRES index 0 to be null; any
 *                  load of selector 0 faults by definition ("null
 *                  selector"). We never use it.
 *   1  kcode 0x9A: present|DPL0|code|readable -- CS lives here.
 *   2  kdata 0x92: present|DPL0|data|writable -- the "ordinary" data
 *                  segment: DS/ES/FS/GS all use it.
 *   3  kstack 0x92: a STACK SEGMENT IS A WRITABLE DATA SEGMENT -- same
 *                  access byte as kdata (writable data, grows up, DC=0).
 *                  We still give it its OWN entry because the spec demands
 *                  six NAMED segments and wants SS to point at its own
 *                  selector: that lets a future kernel switch stacks by
 *                  writing a new SS (which is checked against THIS
 *                  descriptor's DPL) instead of reusing kdata.
 *   4  ucode 0xFA: DPL3 code -- the selector (0x23) carries RPL 3, so the
 *                  "user" half of ring 3 is complete even without ring 0.
 *   5  udata 0xF2: DPL3 writable data.
 *   6  ustack 0xF2: DPL3 writable data, SS-of-ring-3 (for future use).
 *
 * FLAGS 0xC0 everywhere except the null entry: G=1 (4 KiB granularity,
 * which is what makes limit 0xFFFFF mean 4 GiB) and D/B=1 (32-bit
 * segment: 32-bit default operand size and 32-bit addressing -- we are on
 * an i386, and setting D=0 would silently break every 32-bit access).
 */
void gdt_install(void)
{
    /* The seven descriptors, in spec order, all flat (base 0, limit 0xFFFFF
     * where the limit applies). set_gate() splits the fields across the
     * packed struct; the call sites stay human-readable. */
    set_gate(0, 0, 0, 0x00, 0x00);             /* 0: null (required)          */
    set_gate(1, 0, 0xFFFFF, 0x9A, 0xC0);       /* 1: kernel code   sel 0x08 */
    set_gate(2, 0, 0xFFFFF, 0x92, 0xC0);       /* 2: kernel data   sel 0x10 */
    set_gate(3, 0, 0xFFFFF, 0x92, 0xC0);       /* 3: kernel stack  sel 0x18 */
    set_gate(4, 0, 0xFFFFF, 0xFA, 0xC0);       /* 4: user code     sel 0x23 */
    set_gate(5, 0, 0xFFFFF, 0xF2, 0xC0);       /* 5: user data    sel 0x2B */
    set_gate(6, 0, 0xFFFFF, 0xF2, 0xC0);       /* 6: user stack   sel 0x33 */

    /* The GDTR record: the 6 bytes LGDT will read (see src/gdt_flush.asm).
     * limit = table size in bytes, MINUS ONE (a byte offset, not a count:
     * the last valid byte of 56 bytes is at offset 55); base = the address
     * of the first descriptor. `static` keeps the record in the BSS of the
     * kernel image (1 MiB+): that is fine, because only its ADDRESS is
     * passed to the assembly -- LGDT does a plain memory read, and any
     * physical address below 4 GiB is reachable through GRUB's flat GDT.
     * It is NOT the GDT table that needs to live at 0x800. */
    static struct gdt_ptr gp = {
        .limit = GDT_ENTRIES * 8u - 1u,   /* 7 * 8 - 1 = 55                 */
        .base  = GDT_ADDR,                /* physical 0x800                 */
    };

    /* ------------------------------------------------------------------
     * HUMAN-READABLE SUMMARY: the assignment wants the installed table
     * PRINTED. Every field below is read BACK from gdt[] -- the bytes
     * physically written at 0x800 -- not from the constants we passed to
     * set_gate(). If the print ever disagrees with the table in the code,
     * the pointer above (or the struct layout) is the suspect.
     *
     * Column meanings, bit by bit (full table in kernel.h):
     *   sel    : the 16-bit selector (index * 8 | RPL) the CPU stores in
     *            a segment register; user entries carry RPL 3 in bits 1-0.
     *   access : byte 5, printed in hex: P(7) DPL(6-5) S(4) E(3) DC(2)
     *            RW(1) A(0). 9A/92/FA/F2 = the four classic values.
     *   G/D    : the top nibble of byte 6: G=4 KiB granularity, D=32-bit
     *            data / B=32-bit code. Printed so the G=1 trick (limit
     *            0xFFFFF -> 4 GiB) is visible, not just asserted.
     *   role   : which spec segment this descriptor is.
     * ---------------------------------------------------------------- */
    static const uint16_t selectors[GDT_ENTRIES] = {
        0x0000, SEL_KCODE, SEL_KDATA, SEL_KSTACK,
        SEL_UCODE, SEL_UDATA, SEL_USTACK
    };
    static const char *const roles[GDT_ENTRIES] = {
        "null descriptor (required by the CPU, never used)",
        "kernel code   (RPL 0)",
        "kernel data   (RPL 0)",
        "kernel stack  (RPL 0, writable data)",
        "user code     (RPL 3)",
        "user data     (RPL 3)",
        "user stack    (RPL 3, writable data)"
    };

    printk("GDT summary: physical %u, %u descriptors, %u bytes\n",
           (unsigned)GDT_ADDR,
           (unsigned)GDT_ENTRIES,
           (unsigned)(GDT_ENTRIES * 8u));
    for (uint32_t i = 0; i < GDT_ENTRIES; i++) {
        printk("  [%u] sel=", (unsigned)i);
        printk_hex((uint32_t)selectors[i], 4);
        printk("  access=");
        printk_hex((uint32_t)gdt[i].access, 2);
        printk("  G/D=");
        /* The top 4 bits of byte 6: shift the flags|limit-high byte down
         * to read G (bit 3) and D/B (bit 2) without touching the 4 limit
         * bits that share the byte. */
        printk_hex((uint32_t)(gdt[i].flags_limit_high >> 4), 2);
        printk("  %s\n", roles[i]);
    }

    /* The hand-off to assembly: lgdt the record we just built, reload the
     * segment registers, far-jump CS. After this returns, the CPU is
     * running THROUGH our table, and the summary above now describes the
     * live hardware. */
    gdt_flush((uint32_t)&gp);

    printk("GDT installed: CS=%x DS=%x SS=%x, all through our table at %x\n",
           (uint32_t)SEL_KCODE, (uint32_t)SEL_KDATA,
           (uint32_t)SEL_KSTACK, GDT_ADDR);
}
