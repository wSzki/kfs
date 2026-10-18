
; boot.asm — the very first code that runs in our kernel.
; QEMU/GRUB jumps here. Nothing is set up for us yet: not even a stack.

MAGIC    equ 0x1BADB002          ; "this file is a Multiboot kernel"
FLAGS    equ 0                    ; no special requests
CHECKSUM equ -(MAGIC + FLAGS)     ; magic + flags + checksum must equal 0

section .multiboot                ; bootloader looks for this in the first 8 KB
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss                      ; uninitialized data
align 16
stack_bottom:
    resb 16384                    ; reserve 16 KiB for our stack
stack_top:

section .text                     ; executable code goes here
global _start                     ; make the symbol visible to the linker
extern kmain                      ; kmain lives in kernel.c — another file
_start:
    mov esp, stack_top            ; the stack grows DOWN -> start at the top
    call kmain                    ; now C can run

.hang:                            ; kmain "returns" here (it shouldn't)
    hlt                           ; stop the CPU
    jmp .hang                     ; if it ever wakes, stop again
