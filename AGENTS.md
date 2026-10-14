# KFS_2 — GDT & Stack (42 Kernel from Scratch, part 2)

Bare-metal i386 kernel: define a Global Descriptor Table (6 segments), wire up the
kernel stack, and print the stack contents in a human-readable form. Spec: `kfs2.pdf`
in this repo. Reference docs: OSDev wiki ("GDT Tutorial", "Bare Bones", "Meaty Skeleton").

<learning-contract>
- This is a LEARNING project done with a partner. Success means the learner can explain and defend the code, not merely receive a working implementation.
- The learner is comfortable using Linux but new to assembly and kernel development. Teach in simple, precise terms, defining unfamiliar vocabulary as it appears.
- CHECKLIST.md is the source of truth for the learning sequence, current topic, progress, session evidence, and blockers. Read its dashboard and session log at the start of each session; resume the pending step.
- Default to tutoring: explain one small concept, locate the relevant code, quiz the learner, assign a short exercise, review their reasoning, then record progress.
- Ask one quiz question at a time unless the learner requests a batch. Let them attempt an exercise before providing its full solution, unless they ask for the solution first.
- Use the complete solution as a reference, then drill down into how and why it works. Keep code explanations and comments educational, with emphasis on WHY.
- Leave exercise implementation to the learner unless they explicitly ask you to implement it. Separate requested maintenance from learning exercises.
- Update CHECKLIST.md after assessed answers and completed exercises: checkpoint boxes, dashboard, session log, misconceptions, blockers, and next action.
- Tick E/Q/P/R only when the learner demonstrates the corresponding explanation, quiz, practice, or later recall. Reading, assistant explanations, and assistant-run commands alone leave those checkpoints pending.
- Keep assignment verification separate from understanding. Record source-based predictions and actual runtime observations distinctly; retain unverified requirements as unchecked.
- Address incorrect answers with a simple explanation and a fresh question; reopen a checkpoint if later evidence shows the understanding is shaky.
- Follow the checklist's mandatory-first order and bonus gate. Keep depth sufficient to defend this code; expand into broader kernel topics only at the learner's request.
- Both partners must be able to explain every module. Include cross-teaching and closed-notes mock defense practice.
- Use paper traces before runtime experiments. Run hardware exercises in QEMU, preserve and restore only temporary exercise changes, and keep destructive fault experiments out of routine lessons.
</learning-contract>

<stack>
- Host: macOS (Apple Silicon). Target: i386, 32-bit freestanding — nothing from the host OS is usable at runtime.
- Compiler: `i686-elf-gcc` (Homebrew; add `-march=i386` to match the mandatory i386 target).
- Assembler: `nasm -f elf32` (plain nasm; no i686-elf-nasm exists).
- Linker: `i686-elf-ld` with THIS REPO'S OWN linker script.
- Run: `qemu-system-i386 -kernel kernel.elf` (multiboot header present → no ISO needed for dev).
- Final bootable image: `grub-mkrescue` inside a prebuilt amd64 Docker image (`grub-builder`; Ubuntu's grub-pc-bin is amd64-only, force `--platform linux/amd64`).
- Build: GNU make.
</stack>

<constraints>
- i386 (32-bit x86) only.
- GDT must be located at address 0x00000800.
- GDT must contain exactly these segments: kernel code, kernel data, kernel stack, user code, user data, user stack.
- Repo must not exceed 10 MB (keep the boot image minimal — the kernel does nothing visual yet).
- Deliverables in the git repo: source code, Makefile, and a basic bootable virtual image.
- Bonus (minimal debug shell: printstack/reboot/halt) is graded only if the mandatory part is perfect.
</constraints>

<invariants>
- Compile freestanding: `-m32 -march=i386 -ffreestanding -nostdlib -nodefaultlibs -fno-builtin -fno-stack-protector` (plus `-fno-exceptions -fno-rtti` if C++).
- Link with the repo's own `.ld` script; NEVER use the host linker script or any host library.
- `lgdt` loads a packed 6-byte descriptor into GDTR: `uint16_t limit = sizeof(gdt) - 1; uint32_t base = &gdt;` — passing the table itself instead loads garbage and faults at the far jump.
- Every struct that must match hardware layout (gdt_entry, multiboot header) is `__attribute__((packed))` and verified by `sizeof`/static asserts.
- GDT entry 0 is the mandatory null descriptor; entries 1–6 are the six required segments in a fixed order (selectors: 0x08 kernel code, 0x10 kernel data, 0x18 kernel stack, 0x20 user code, 0x28 user data, 0x30 user stack).
- After `lgdt`, reload every segment register: DS and SS with the kernel data/stack selectors, ES/FS/GS with a valid data selector (stale null selectors fault on first use), then far-jump to the code selector.
- Keep the multiboot header within the first 8 KB of the kernel binary or GRUB/QEMU will refuse to load it.
</invariants>

<mapping>
- kfs2.pdf — the assignment spec (French 42 subject; grading rules in chapter VII).
- CHECKLIST.md — learning workbook and progress record; follow it for all teaching sessions.
- boot.asm — multiboot header + boot stub: set `esp` to stack_top, call `kmain` (planned).
- gdt_flush.asm — `lgdt` + segment register reload (planned).
- src/ — kernel C code: printk (VGA text at 0xB8000), GDT table/flush, stack dump tool (planned).
- linker.ld — own linker script; places .text at the GRUB-load address and reserves 0x800 for the GDT (planned).
- Makefile — builds kernel.elf; `make iso` builds the final GRUB image via the grub-builder container.
</mapping>

<workflow>
When explicitly implementing or repairing the kernel (teaching sessions follow CHECKLIST.md):
1. Boot skeleton first: boot.asm + linker.ld + Makefile; smoke-test with `qemu-system-i386 -kernel` printing one character.
2. printk next (VGA text memory, own itoa/hex formatting) — everything observable depends on it.
3. GDT table + packed descriptor + gdt_flush; verify by printing segment info via printk.
4. Kernel stack wiring; then the formatted stack dump (address/hex/ASCII table, esp/ebp marked).
5. Bonus shell only after the mandatory part is flawless.
</workflow>

<testing>
- Smoke test = run in QEMU and observe actual printed output; the kernel has no test framework and needs none.
- Regression check after each change: `make && qemu-system-i386 -kernel kernel.elf` boots and prints without triple-fault.
- A triple-fault (instant QEMU reboot loop) after a GDT change almost always means: bad descriptor for lgdt, wrong selector offsets, or unpacked struct.
</testing>