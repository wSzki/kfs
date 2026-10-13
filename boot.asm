;=============================================================================
; boot.asm -- the FIRST three lines of the KFS_2 kernel.
; ============================================================================
;
; ROLE IN THE BOOT CHAIN
; ----------------------
; The CPU arrives at this file with NOTHING useful set up by us:
;
;   BIOS -> GRUB -> (multiboot handshake) -> _start  <- YOU ARE HERE
;
; This file does three jobs, in order:
;
;   (a) DECLARES the multiboot header. GRUB scans the first 8 KiB of the
;       binary for the magic 0x1BADB002; if it finds it, it knows how to
;       load us (ELF, at our linker-chosen address, in 32-bit mode).
;
;   (b) PROVIDES _start, the symbol GRUB jumps to. It sets up the kernel
;       stack and transfers control to kmain() (src/kmain.c) with the two
;       registers GRUB handed us, converted into C arguments.
;
;   (c) RESERVES the kernel stack: a 16 KiB region of .bss, 16-byte
;       aligned, exported as the symbols stack_bottom / stack_top that
;       include/kernel.h declares `extern` and that stack_dump() walks.
;
; CONCEPTS INTRODUCED HERE (the ones a 386 kernel student must have)
; -------------------------------------------------------------------
;   - What multiboot is and why the header must be within the first 8 KiB
;   - The checksum math (magic + flags + checksum == 0, mod 2^32)
;   - What GRUB guarantees at entry (mode, GDT, EAX/EBX) and what it does
;     NOT guarantee (a stack!)
;   - Why the stack must exist before ANY `call` or `push` in C code
;   - The cdecl calling convention (right-to-left argument pushing)
;
; FURTHER READING (the OSDev pages these comments are built on)
;   - https://wiki.osdev.org/Multiboot        the protocol, header, info struct
;   - https://wiki.osdev.org/Multiboot_Information  the struct EBX points to
;   - https://wiki.osdev.org/Bare_Bones       the whole skeleton we are in
;   - https://wiki.osdev.org/Stack            why a stack is sacred on x86
;   - https://en.wikipedia.org/wiki/Cdecl     how C passes arguments on x86
;
; TOOLCHAIN NOTE: assembled with `nasm -f elf32` (see the Makefile).
; The output is a 32-bit ELF object, linked by i686-elf-ld with
; linker.ld into kernel.elf. GRUB then loads that ELF.
;=============================================================================

bits 32        ; Target ISA: 32-bit protected mode. Everything in this
               ; kernel is 32-bit; there is no real-mode code here because
               ; GRUB already performed the 16-bit -> 32-bit switch for us.


;-----------------------------------------------------------------------------
; PART (a): THE MULTIBOOT HEADER
;-----------------------------------------------------------------------------
; WHAT IS MULTIBOOT?
;   Multiboot is a BOOTLOADER-TO-KERNEL contract: the kernel declares "I am
;   a multiboot kernel, load me like this" by placing a 12-byte header
;   inside its own binary, and the bootloader (GRUB) promises a precise
;   machine state at the jump into the kernel entry point. It decouples
;   kernels from a specific bootloader: GRUB, or any other multiboot
;   loader, can start our kernel without knowing its internals.
;
;   The contract, from the kernel's point of view, is tiny:
;     1. The header sits inside the binary (so GRUB can find it).
;     2. GRUB loads the binary, switches the CPU to 32-bit protected
;        mode, and jumps to our entry symbol (the ELF entry point, which
;        the linker script sets to _start) with:
;            EAX = 0x2BADB002        the "bootloader magic"
;            EBX = pointer to the multiboot info structure
;     3. That is ALL we may rely on. Everything else we build ourselves.
;
; WHY MUST THE HEADER BE WITHIN THE FIRST 8 KiB OF THE BINARY?
;   GRUB does not parse the whole ELF to find us; it SCANS the first
;   8192 bytes of the loaded image for the 4-byte magic 0x1BADB002 (an
;   explicit rule of the multiboot spec). If the
;   header is not there, GRUB cannot classify our file as multiboot and
;   refuses to load it ("no multiboot header found").
;
;   This is exactly why linker.ld forces the layout:
;       .text : { KEEP(*(.multiboot)) ... }
;   i.e. our .multiboot section is emitted as the VERY FIRST bytes of the
;   image (at the 1 MiB load address). KEEP() additionally prevents the
;   linker from dropping the section as "dead code" -- it has no entry
;   point and no references, so without KEEP it could silently vanish and
;   the kernel would stop booting with the same cryptic GRUB error.
;
; THE THREE WORDS OF THE HEADER
;   The header is three 32-bit words: magic, flags, checksum. Alignment:
;   the magic must be 32-bit aligned within the file; our linker places
;   it at offset 0 of the image (1 MiB in memory), so `align 4` holds
;   trivially, but we write it anyway -- it is the spec's invariant and
;   it keeps the file honest if the layout ever changes.
;
;   flags = 0 says: "I request NONE of the optional features". The flag
;   bits mean: bit 0 (0x1) = "align all boot modules on page boundaries",
;   bit 1 (0x2) = "also pass me the physical memory map in the info
;   struct", bit 16 = "use the A.OUT kludge layout instead of ELF".
;   flags = 0 is the simplest correct choice for this project: GRUB still
;   fills EBX with an info struct we can print.
;
; THE CHECKSUM MATH
;   magic + flags + checksum must equal zero modulo 2^32 (that is, the
;   sum wraps around the 32-bit word and lands on 0x00000000). So:
;
;       checksum = -(magic + flags)  mod 2^32
;                = -(0x1BADB002 + 0)
;                = -0x1BADB002
;                = 2^32 - 0x1BADB002
;                = 0xE4524FFE
;
;   Verify by hand (32-bit, the low byte of 0x1BADB002 is 0x02 so the
;   result must end in 0xFE ... the full addition):
;         0x1BADB002
;       + 0xE4524FFE
;       = 0x1_00000000   == 0x00000000 (mod 2^32)          [OK]
;
;   It is two's complement: for a word W, -W mod 2^32 is 0xFFFFFFFF - W +
;   1. A bootloader that sees a wrong checksum concludes the header is
;   garbage and does NOT load the kernel -- the #1 "why doesn't it boot"
;   of multiboot projects, right after a misplaced header.
;-----------------------------------------------------------------------------
section .multiboot
    align 4
    dd  0x1BADB002      ; magic: the only thing GRUB actually searches for
    dd  0x00000000      ; flags: v1 kernel, no memory map requested
    dd  0xE4524FFE      ; checksum: -(0x1BADB002 + 0) mod 2^32 (math above)
                         ; NOTE: do not confuse this with multiboot *2*,
                         ; which uses a DIFFERENT magic (0xE85250D6) and a
                         ; completely different header layout. We use
                         ; multiboot v1: it is what QEMU's -kernel shortcut
                         ; and every GRUB build support out of the box.


;-----------------------------------------------------------------------------
; PART (b): _start -- GRUB's jump target, the bridge into C
;-----------------------------------------------------------------------------
; WHAT GRUB GUARANTEES AT ENTRY (worth memorizing, it is the whole
; "state machine" of a bare boot):
;
;   +  The CPU is in 32-bit PROTECTED MODE. GRUB switched it; we never
;      saw the 16-bit transition.
;   +  A temporary FLAT GDT is loaded: base 0, limit 4 GiB, so every
;      physical address below 4 GiB is reachable through a plain pointer
;      (linear == physical). That is why we can write to 0xB8000 (VGA)
;      and 0x3F8 (serial) immediately. This GDT is GRUB's, not ours:
;      gdt_install() (src/gdt.c) replaces it with our table at 0x800.
;   +  EAX = 0x2BADB002 (the multiboot magic -- proof that a multiboot
;      bootloader, not something random, started us).
;   +  EBX = the physical address of the multiboot info structure (the
;      memory map, boot device, etc.).
;   +  Interrupts: state is unspecified-ish; we assume "possibly on".
;
; WHAT GRUB DOES NOT GIVE US -- and therefore what this stub MUST do:
;
;   * A KERNEL STACK. ESP holds whatever GRUB left in it. The x86 CPU
;     takes every `push`, `call` and interrupt by writing through
;     SS:ESP. If we let C code (or an interrupt!) write through a
;     garbage stack pointer, it corrupts random memory and, within a
;      few instructions, faults into a triple fault -> QEMU reboot loop.
;     So the stack is set up here, the moment we are alive, BEFORE any
;     push/call in the entire program.
;
;   * Interrupts OFF. Our first job is `cli`. If an interrupt fires
;     between _start and the `mov esp`, the CPU would push its handler
;     frame onto the (nonexistent) stack and die. With interrupts off
;     we have total control; the rest of the kernel keeps them off and
;     polls the keyboard instead, so no IDT is needed at all this week.
;
; THE CALL INTO C: THE CDECL CONTRACT
;   kmain's signature (include/kernel.h) is:
;       void kmain(uint32_t magic, uint32_t mbi_addr);
;   On i386, C arguments are passed on the stack in the CDECL calling
;   convention, and CDECL pushes arguments RIGHT-TO-LEFT: the LAST
;   parameter goes onto the stack FIRST. Why right-to-left? Because the
;   C language allows variadic functions (printf-style, any number of
;   extra args), and the callee cannot know how many bytes to pop -- so
;   the CALLER must build the frame, and a fixed first address for the
;   callee's first parameter requires that it be pushed LAST, ending up
;   at a constant offset from ESP. (Variadics are why printf works at
;   all; see the cdecl page for the full picture.)
;
;   So for kmain(magic, mbi_addr):
;       1st push  : mbi_addr  (last parameter) -> arrives at [ESP]
;       2nd push  : magic     (first parameter) -> arrives at [ESP+4]
;       `call kmain` then pushes the return address -> [ESP+4]
;   and kmain sees its first argument at ESP+4, second at ESP+8, exactly
;   where i386's C ABI says a cdecl caller must have put them. Pushing
;   in the wrong order would make kmain read magic/mbi_addr swapped --
;   a classic "the numbers are reversed" bug that compiles and links
;   cleanly and only shows up as nonsense printed values.
;
;   (The caller cleans up? No -- the caller's frame here is never used
;   again: kmain never returns to _start, it ends in shell_run() which
;   never returns. The `hlt` loop below is only a belt-and-braces net.)
;-----------------------------------------------------------------------------
section .text
    global _start

_start:
    cli                   ; 1. interrupts off, NOW, before any stack use.
                          ;    A stray interrupt landing between _start
                          ;    and `mov esp` would push a handler frame
                          ;    onto whatever ESP happens to point at ->
                          ;    instant corruption. (Classic bug #1.)

    mov esp, stack_top    ; 2. give the CPU OUR stack: ESP starts one
                          ;    past the top of the 16 KiB region in .bss
                          ;    (i386 stacks grow DOWN, so "start at the
                          ;    top" = start at the highest address). From
                          ;    here on, every push/call lands in known
                          ;    good memory. MUST precede every push that
                          ;    follows -- C code cannot run without a
                          ;    stack, and this is the one instruction
                          ;    that makes it possible.

    push ebx              ; 3. argument 2 of kmain: the multiboot info
                          ;    pointer. Pushed FIRST because CDECL is
                          ;    right-to-left: the LAST parameter goes on
                          ;    the stack first.

    push eax              ; 4. argument 1 of kmain: the multiboot magic
                          ;    0x2BADB002. Pushed SECOND, so it sits at
                          ;    [ESP] + 4 after the call -- the exact
                          ;    layout kmain's compiled code expects.

    call kmain            ; 5. jump into C. The CPU pushes the return
                          ;    address (into _hang, since kmain in fact
                          ;    never returns), then runs kmain() at the
                          ;    address the linker gave it.

; kmain does not return (shell_run() polls the keyboard forever). This
; loop is only executed if someone makes kmain return -- then we park the
; CPU as politely as bare metal allows:
_hang:
    cli                   ; no more interrupts can preempt us
    hlt                   ; stop the CPU until the next interrupt
                          ; (there is none, interrupts are off): a
                          ; zero-power, zero-CPU-burn "halt the machine"
    jmp _hang             ; hlt wakes on any pending interrupt flag;
                          ; loop back and re-halt. The kernel is done;
                          ; this is the standard kernel "hang" idiom.


;-----------------------------------------------------------------------------
; PART (c): THE KERNEL STACK IN .bss
;-----------------------------------------------------------------------------
; LAYOUT, one 16 KiB region, i386 growing-down style:
;
;      high addresses (memory)
;      +-------------------+ <- stack_top = stack_bottom + 16384
;      |                   |    ESP is initialized HERE at boot.
;      |   free .bss space |
;      |   (zeros at boot) |    16384 usable bytes, used downward.
;      +-------------------+ <- stack_bottom (lowest address)
;
; WHY 16 KiB AND WHY 16-ALIGNED? The subject (kfs2.pdf) asks for a
; 16 KiB kernel stack; 16-byte alignment is the ABI-recommended stack
; boundary on x86-32 (it also keeps any future SSE alignment assumptions
; cheap). `alignb 16` pins the BOTTOM of the region to a 16-byte
; boundary; the size (16384) is itself a multiple of 16, so the TOP is
; 16-aligned too -- and that matters, because that is where ESP starts.
;
; !! SYNC WITH THE HEADER !! The size of this region must equal
; KERNEL_STACK_SIZE in include/kernel.h:
;     #define KERNEL_STACK_SIZE 16384u
; if you change one, change BOTH. stack_dump() (src/stack_dump.c) walks
; between `esp` and `stack_top` and knows the region's extent from that
; #define -- a mismatch means the dump tool reads past the real stack
; region or misses part of it.
;
; EXPORTING THE SYMBOLS: `global` makes the assembler emit stack_bottom /
; stack_top as exported (T) symbols in the object file, so the linker can
; resolve the `extern uint8_t stack_bottom[]; stack_top[];` declarations
; in include/kernel.h. This is the assembly half of a split-brain pair:
; the header declares, this file defines. Missing `global` here is the
; classic "undefined reference to `stack_top' at link time" bug.
;-----------------------------------------------------------------------------
section .bss
    global stack_bottom, stack_top

    alignb 16                 ; bottom of the region on a 16-byte boundary
stack_bottom:
    resb 16384                ; reserve 16 KiB (== KERNEL_STACK_SIZE).
                              ; resb allocates bytes in .bss without
                              ; putting them in the file: the ELF marks
                              ; the section "bss" and the bootloader/OS
                              ; must zero it before use. (GRUB does.)
stack_top:                    ; = stack_bottom + 16384. One PAST the top:
                              ; this is the exact value boot.asm loads
                              ; into ESP, and the "exclusive end" that
                              ; stack_dump() prints up to.
