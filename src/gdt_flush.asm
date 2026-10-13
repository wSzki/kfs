; src/gdt_flush.asm -- the one instruction C cannot do: load our GDT.
; ============================================================================
;
; ROLE IN THE BOOT CHAIN
; ----------------------------------------------------------------------------
; gdt_install() (src/gdt.c) already WROTE the 56 bytes of the table to
; physical 0x800 and printed a summary of them. But writing bytes into
; memory changes NOTHING for the CPU: until the moment LGDT + the segment
; register reloads below execute, the CPU is still fetching descriptors
; from GRUB's temporary GDT. This file performs that hand-off:
;
;   1. LGDT [gp]      -- tell the CPU "your GDT lives at the address in
;                        gp" (gp is the 6-byte struct gdt_ptr).
;   2. reload DS/ES/FS/GS with the kernel data selector,
;      reload SS with the kernel STACK selector,
;   3. FAR JUMP to the kernel code selector -- the ONLY way to change CS.
;
; WHY ASSEMBLY AT ALL
; ----------------------------------------------------------------------------
; The C compiler cannot express three things that the CPU needs:
;   - LGDT is an instruction that has no C builtin (GCC knows no lgdt).
;   - CS CANNOT be written with MOV. `mov cs, reg` is an illegal
;     instruction (#UD). The only two ways to change CS are the far jump
;     and the far call, and both are assembly.
;   - A far jump encodes a full selector:offset pair in the instruction
;     stream; that is not expressible in C at all.
; So gdt_flush() is the bridge: C prepares everything, one assembly
; function does the hardware ceremony, and C resumes on the other side.
;
; CALLING CONVENTION: cdecl, ONE argument
; ----------------------------------------------------------------------------
; The caller (gdt_install in src/gdt.c) pushes exactly one 32-bit value:
; the ADDRESS of a struct gdt_ptr -- i.e. &(struct gdt_ptr){ limit=55,
; base=0x800 }. At the moment `gdt_flush:` executes, the stack is:
;
;    [esp+0] : the return address (the `call` pushed it)
;    [esp+4] : OUR ARGUMENT = the address of the gdt_ptr record
;
; So the argument lives at [esp+4], NOT [esp] -- that is a cdecl call,
; exactly like a C function with one parameter.
;
; THE CLASSIC TRIPLE-FAULT BUG: passing the table instead of the record
; ============================================================================
; LGDT does not load "a GDT". It reads SIX BYTES from the address in its
; operand and loads them into the GDTR register, which has exactly this
; hardware layout:
;
;   GDTR:  [15:0]  LIMIT : size of the GDT in bytes, MINUS ONE
;         [31:16] BASE  : 32-bit physical address of the first descriptor
;
; ...and struct gdt_ptr mirrors that byte-for-byte, which is why
; gdt_flush wants `&gp` (the ADDRESS of the 6-byte record). The classic
; mistake is passing `gdt` itself -- the address of the 56-byte TABLE.
; Then the six bytes LGDT reads are the first 6 bytes of descriptor 0
; (all zero for a null entry) and the low byte of descriptor 1's base:
; GDTR.limit = 0x0000 (a one-byte GDT!), GDTR.base = 0x00000000 (the
; null entry, all zero). Every subsequent segment register load finds a
; "limit of zero, not present" descriptor, faults, the fault handler
; itself faults, and the CPU gives up with a TRIPLE FAULT: instant
; reboot loop, nothing printed, a kernel that died with no clue. Passing
; the record instead of the table is the single most common GDT bug, and
; it is the one this function's comment block exists to stop you making.
;
; FURTHER READING
; ----------------------------------------------------------------------------
;   - OSDev wiki "GDT Tutorial": https://wiki.osdev.org/GDT_Tutorial
;     (the whole lgdt + reload + far jump sequence, with the "why far
;     jump" question answered at length)
;   - Intel SDM Vol. 3, ch. 5.2 "Loading Segment Registers" -- LGDT, and
;     exactly when a descriptor is fetched (it is fetched on the RELOAD,
;     not on the LGDT itself).
; ============================================================================

    [bits 32]
    section .text

; Export the symbol to the linker so src/gdt.c's `gdt_flush(...)` call
; resolves to this label: C `extern` declarations and nasm `global` are
; the two halves of the same hand-off (in C++ you would additionally
; need `extern "C"` to stop name mangling -- a C project does not).
    global gdt_flush

gdt_flush:
; ------------------------------------------------------------------------
; Step 1: take the argument.
; [esp+4] holds the address of the 6-byte struct gdt_ptr record that
; gdt_install() built on its own stack. EAX is just a scratch register
; of our choosing: there is no convention, and we do not need it again
; after LGDT.
;
; Note the 32-bit access: the argument is a 32-bit address, so read it
; as such ([esp+4], 32 bits) -- a 16-bit read would truncate the address
; and point at the wrong physical location.
    mov eax, [esp+4]      ; eax = &gp : address of struct gdt_ptr
; ------------------------------------------------------------------------
; Step 2: load the GDTR.
; LGDT [eax] reads SIX BYTES from physical address eax+0..5:
;   bytes 0-1 -> GDTR.limit = 55   (7 descriptors * 8 - 1)
;   bytes 2-5 -> GDTR.base  = 0x800
;
; THE KEY INSIGHT: LGDT BY ITSELF CHANGES NOTHING OBSERVABLE. It only
; re-points a register that the CPU uses to LOOK UP descriptors. The CPU
; still holds the OLD selectors in CS/DS/SS, and for a while it happily
; uses the OLD descriptors behind them. Until every segment register is
; RELOADED, we are still running on GRUB's table. (Reloading is what
; actually makes the CPU fetch the NEW descriptor from 0x800.)
    lgdt [eax]            ; load GDTR from the 6-byte record at [eax]
; ------------------------------------------------------------------------
; Step 3: reload the DATA segment registers.
; 0x10 is SEL_KDATA (index 2, RPL 0): the kernel data descriptor. All
; four of DS/ES/FS/GS now use the SAME selector -- and they behave
; identically for a reason worth pausing on:
;
;   "FLAT" means base 0, limit 4 GiB. A memory access through ANY of
;   these registers computes: physical = base + offset = 0 + offset =
;   offset. So DS, ES, FS, GS are all the SAME window onto memory; the
;   distinction between them is only which register you happen to have
;   set the prefix from. For now (single task, no processes) they are
;   interchangeable; they earn their keep when multiple processes share
;   one kernel but get per-process DS/ES... which is not what we have.
;
; mov ax, not mov eax: segment registers are 16-bit, so the 16-bit form
; is correct and the high 16 bits of EAX are simply not read. No hazard.
    mov ax, 0x10          ; SEL_KDATA = 0x10: kernel data descriptor
    mov ds, ax            ; DS: default data segment (most loads/stores)
    mov es, ax            ; ES: string instructions (MOVS, LODS, STOS, ..)
    mov fs, ax            ; FS: "spare" data segment (used by the kernel
                          ;     when it wants to point somewhere else,
                          ;     e.g. per-task data in later kernels)
    mov gs, ax            ; GS: same idea as FS; the CPU has exactly one
                          ;     of each, and all four are now ours.
; ------------------------------------------------------------------------
; Step 4: reload the stack segment.
; 0x18 is SEL_KSTACK (index 3, RPL 0) -- NOT SEL_KDATA. We deliberately
; exercise the kernel-stack entry here: SS is the ONLY register whose
; descriptor is checked on EVERY PUSH/POP/CALL/RET, not just on writes,
; so pointing it at ITS OWN descriptor is what "uses" that segment.
;
; Two subtleties:
;   - Writing SS directly with MOV is SAFE as long as we do not change
;     ESP in the same breath: the CPU checks SS's DPL against the
;     current privilege (still 0, and the descriptor is DPL 0, so it
;     passes). The danger is in switching to a NEW ESP and a NEW SS at
;     the same time (that is the "far stack" dance of later kernels,
;     not this one).
;   - The descriptor's DC (direction) bit is 0: stacks grow DOWN on
;     x86, so DC=0 is what a stack segment demands. A stack segment IS
;     a writable data segment -- that is why its access byte is 0x92,
;     the same as kdata, not a special "stack" type.
    mov ax, 0x18          ; SEL_KSTACK = 0x18: kernel stack descriptor
    mov ss, ax            ; SS: the stack segment; PUSH/POP/RET check it
; ------------------------------------------------------------------------
; Step 5: the FAR JUMP -- and the only way to reload CS.
;
; WHY A FAR JUMP IS REQUIRED (two independent reasons, both real):
;
;   (a) CS CANNOT BE WRITTEN. There is no `mov cs, reg` -- the ISA
;       simply does not allow writing CS as a plain register; it is one
;       of a handful of control registers that only specific instructions
;       touch. The two instructions that set CS are the FAR jump and the
;       FAR call, and the jump is the one we want (a far CALL would also
;       push a far return address we never intended).
;
;   (b) THE CPU PREFETCHES INSTRUCTIONS. While the far jump is decoding,
;       the CPU's out-of-order pipeline may already have fetched a chunk
;       of instructions THROUGH THE OLD CS (GRUB's code descriptor).
;       Without the far jump, those prefetched bytes would keep executing
;       even after "CS = ours" -- a silent, horrible corruption where the
;       next few instructions are actually the previous code's. The far
;       jump does two atomic things: it loads CS with the new selector,
;       AND it flushes the instruction queue / pipeline so that the next
;       instruction is fetched through the NEW descriptor. One instruction,
;       and the CPU is unambiguously running OUR code through OUR CS.
;
; `jmp 0x08:.flushed` encodes selector 0x08 (SEL_KCODE: index 1, RPL 0)
; and the LOCAL label .flushed as a displacement from the far jump's own
; address. That is a "far jump within the kernel": it changes CS to the
; selector we name (0x08) while continuing at the .flushed instruction
; right where we are. After this instruction, CS = 0x08, and every next
; instruction fetch is looked up as: descriptor index 1 of OUR table --
; the descriptor that LIVES at physical 0x800 (the CODE itself stays at
; its normal address around 1 MiB; only the descriptor record sits at
; 0x800).
    jmp 0x08:.flushed     ; FAR jump: CS <- 0x08 (SEL_KCODE), flush prefetch
                          ;   queue, start executing .flushed through OUR CS
.flushed:
; Step 6: return to C.
; `ret` pops the 32-bit return address that `call gdt_flush` pushed. At
; this point every segment register points into OUR table, and gdt_install()
; resumes on the C side, confident that the hardware matches the summary it
; printed. The pipeline now fetches through descriptor 1; the old GRUB
; descriptor is dead.
    ret                   ; pop [esp] into EIP, back in gdt_install()
