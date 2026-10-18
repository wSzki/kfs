# KFS_2 — Learning and Defense Checklist

## Goal and starting point

**Goal:** understand this repository well enough to explain it at a defense, answer follow-up questions, and make small changes deliberately. You do not need to become a kernel specialist or memorize the entire x86 instruction set.

**Starting point:** comfortable using Linux, new to assembly and kernel development. We will explain the missing concepts as they appear. Unfamiliar vocabulary is not a failure; pretending to understand it makes the next lesson harder.

**Working estimate:** 20–30 focused hours for the mandatory material, with extra time if C pointers or build tools need attention. Budget another 3–5 hours for the bonus. These are planning estimates, not pass conditions. A focused hour includes explaining, drawing, predicting, and practicing—not just reading.

This is a workbook, not a completed evaluation report. **No understanding, build, boot, or assignment requirement has been validated yet.** The explanations are based on the sources and subject; lab results must come from actual runs.

## How we will work together

For one small topic at a time:

1. **Explain:** use a small visual (ASCII diagram, memory layout, or flow) plus a concrete example and brief prose; the learner explicitly prefers visual explanations. Define new words and connect them to the code. You can interrupt with questions.
2. **Locate:** we look at a small relevant source section, not an entire heavily commented file at once.
3. **Recall:** close the explanation and answer a few questions in your own words. I ask one question at a time unless you request a larger batch.
4. **Apply:** you draw a diagram, calculate a value, trace instructions, or make a small temporary change. For runtime exercises, predict before running.
5. **Review:** compare prediction with observation. I explain mistakes and ask a different question to check the repaired understanding.
6. **Record:** update the checkpoint boxes and session log with evidence. Revisit the important ideas in a later session.

I will not give you an exercise's full solution before your first attempt unless you ask for it. The lesson explanations necessarily contain the concepts you will need; the questions test whether you can use them without copying.

### What each checkbox means

Each module has four checkpoints:

- **E — Explain:** you can explain the listed concepts in your own words after the lesson.
- **Q — Quiz:** you can answer the core questions without an answer pasted in front of you. Exact wording is not required; correct reasoning is.
- **P — Practice:** you personally complete the exercise and explain its result. A prediction is sufficient only for an explicitly paper-only exercise; runtime exercises require observed output.
- **R — Recall:** at a later session, you answer a fresh question or explain the topic without rereading the lesson first.

Reading a section does not tick E. My explaining it does not tick Q. My running a command does not prove that you can do it yourself. A module is complete only when all four boxes have evidence.

A wrong answer is a diagnostic, not a grade. We record the misconception, reteach the missing piece, and try again. If something previously passed proves shaky, reopen the relevant box and record why.

### Progress dashboard

**Current topic:** M00 introduction session closed with a learner recap. Boot/build/QEMU path and the GDT's range/rule model demonstrated; stack dump needs the “stack memory specifically” qualifier. Subject scope and 32-bit/i386 meaning remain the next teaching targets. **Validated modules:** none; M00-E/Q/P/R remain pending.

| Module | Topic | Progress | Next evidence |
|---|---|---|---|
| M00 | What this project actually does | Assessment in progress | Start next session with brief visual recall; explain host versus i386 target and review subject/bonus/partner scope, then complete the project explanation |
| M01 | Bytes, addresses, registers, and essential C | Not started | Memory/pointer worksheet |
| M02 | The assembly you actually need | Not started | Instruction and stack trace |
| M03 | Boot and the Multiboot handoff | Not started | Boot timeline and header reasoning |
| M04 | Compile, link, and produce a boot image | Not started | Build explanation and two boot paths |
| M05 | Printing without an operating system | Not started | Trace one character; output experiment |
| M06 | GDT concepts and selector arithmetic | Not started | Selector worksheet |
| M07 | Descriptor bytes and installing the GDT | Not started | Descriptor/operand diagrams and load trace |
| M08 | Stack, calls, returns, ESP, and EBP | Not started | Call-frame drawing |
| M09 | Reading and defending the stack dump | Not started | Decode a row; observe a real dump |
| M10 | Put it together and reason about failures | Not started | Boot milestones and cautious diagnosis |
| M11 | Mandatory mock defense and delivery proof | Not started | Closed-notes defense and artifact evidence |
| B01 | Bonus: keyboard input | Not started | Scancode and Shift-state trace |
| B02 | Bonus: shell, reboot, and halt | Not started | Command/buffer trace and emulator exercise |

Treat this table as a summary, not a second independent score. The module boxes and session log are the evidence.

### Session log

Append one row after each session. Name exact checkpoints, for example `M02-Q`, rather than writing “understood assembly.” Summarize your answer or observation so another session can resume from this file.

| Date | Module / checkpoint | Your answer or exercise evidence | Misconception / blocker | Next action |
|---|---|---|---|---|
| 2026-10-14 | M00 introduction; M00-Q1 pending | Workbook and first explanation provided; no learner answer assessed | None assessed yet | Answer M00-Q1 in your own words |
| 2026-10-14 | M00-Q1 partial answer | Answered “an os?”; identifies the operating system, but has not yet named concrete services | Distinction between the OS and the services it provides still to assess; no checkpoints earned | Explain who handles a normal C program's request to print to a Linux terminal |
| 2026-10-14 | M00-Q1 follow-up; partial evidence | Answered “printf wouldnt exist?”; correctly identifies that ordinary hosted printf is not available in this kernel | Library function versus underlying OS service not yet assessed; M00-E/Q remain unchecked | Explain whether copying printf alone would supply the missing output support |
| 2026-10-18 | M00 restart; guided session begun | Learner requests a 30-minute session from the beginning; revisiting hosted output versus kernel output before assessment | No new answer assessed; all checkpoints remain unchecked | Explain the missing support behind ordinary printf, then attempt M00-Q1 |
| 2026-10-18 | M00-Q1 follow-up; partial evidence | Answered “no we dont have libc or anything to run c”; correctly predicts that formatting alone cannot display text | Clarification needed: compiled C can execute without libc; what is missing is library/OS support, not the CPU's ability to execute compiled code. M00-E/Q remain unchecked | Explain whether a compiled C addition needs libc or Linux |
| 2026-10-18 | M00 prerequisite check; correction pending | Answered “linux because there is no imports” to whether compiled add needs Linux or libc | Answer names Linux; “no imports” reasoning is ambiguous. Retaught compiler versus CPU versus library: this addition requires neither Linux nor libc. No checkpoints earned | Identify what executes the addition after compilation and loading |
| 2026-10-18 | M00 prerequisite check; partial evidence | Correctly answered “cpu?” for who performs addition; then proposed compiling outside and giving code to the kernel to read | CPU execution identified; kernel versus bootloader roles still unclear. Explained that we compile the kernel itself, the bootloader loads it, and the CPU executes it; no full module checkpoint earned | Explain whether kernel.elf is our kernel or a separate program handed to it |
| 2026-10-18 | M00 prerequisite clarification demonstrated | Answered “it's our compiled kernel” for kernel.elf, following correct identification of CPU execution | Earlier kernel-as-code-reader confusion resolved for this immediate check; later recall remains untested. Full M00-E/Q still pending | Explain why our running kernel needs its own output implementation rather than Linux terminal services |
| 2026-10-18 | M00-Q1 output follow-up; demonstrated partial target | Explained “we need to instruct it to print stuff to the actual hardware somehow”; correctly identifies the missing device-output implementation. Asked how HDMI relates to VGA text memory | Distinguishing display connector from software-visible hardware interface is next. This answer does not complete the remaining M00 scope questions or E/Q/P/R checkpoints | Explain VGA text memory in QEMU versus the host's display connection, then continue project scope |
| 2026-10-18 | M00 output application; paper reasoning demonstrated | Predicted no printing-code change for moving the QEMU window to HDMI: “VGA is a programming interface, the hardware/emulator handles it?” | Correct for this QEMU setup; clarified that QEMU supplies the emulated VGA interface while the host display system handles the actual screen connection. No runtime observation claimed; full M00 checkpoints remain pending | Introduce GDT and stack roles, then distinguish a boot message from completion of mandatory requirements |
| 2026-10-18 | M00 scope check; not yet demonstrated | Answered “not sure” when asked why booting and printing hello is insufficient | GDT setup and readable stack contents not yet recalled as mandatory goals. Retaught that a boot message demonstrates the reached output path, not completion of these requirements; no checkpoints earned | Identify whether a stack dump shows source code or values stored in stack memory |
| 2026-10-18 | M00 stack-dump scope; immediate check correct | Answered “values stored in memory while the kernel runs,” correctly distinguishing a stack dump from C source code | Stack-dump output purpose identified; stack mechanics, GDT purpose, remaining scope questions, and later recall still unassessed. Full checkpoints remain unchecked | Explain the difference between configuring the GDT and displaying stack contents |
| 2026-10-18 | M00 GDT scope recognition; tutoring adjustment | Correctly identified “to use our GDT?” as the missing task, then said “duh” | Question contained its own answer; recognition is not evidence of an independent explanation. Learner finds this style too obvious; switch to short explanations and meaningful synthesis/application | Attempt a short partner-facing project explanation and identify what remains unclear; assess M00-E/P against actual content |
| 2026-10-18 | M00-E/P project-summary attempt; incomplete | Explained compiling a kernel for a bootloader and writing our own screen output through VGA. Said “GDT is to store our memory? unsure” and “the stack dump shows the memory of the kernel?” | Boot/output summary is sound at this level; GDT stores segment descriptions, not the program's actual working contents. Stack dump inspects the stack region, not all kernel memory. M00-E/P remain unchecked pending corrected explanation | Work through a paper example of a function saving a value on its stack, distinguishing that value from GDT metadata |
| 2026-10-18 | M00 clarification requested; exercise paused | Asked “so it's like an array of pointers in C?” and requested an ASCII diagram before answering the stack-value exercise | Array intuition is useful; distinguish a plain pointer from a descriptor containing base, limit, and access information. No new assessed answer or checkpoint | Show the GDT's seven slots as descriptor records, separate from the memory they describe; leave the paper exercise pending |
| 2026-10-18 | M00 GDT clarification; unresolved | Said “still a bit blurry but ok” after the descriptor-array diagram | Diagram has not established understanding; no checkpoint earned. Pause new material and identify whether contents, purpose, or CPU usage is unclear | Ask which part of the GDT model is unclear; target that gap before resuming the paper exercise |
| 2026-10-18 | M00 vocabulary blocker isolated | Identified “what a descriptor is” as the unclear part | Define the unfamiliar noun directly before adding CPU mechanics: one record containing properties of something; in the GDT, one segment-description entry. No understanding checkpoint assessed yet | Relate one descriptor to a C struct instance and the GDT to an array of those instances |
| 2026-10-18 | M00 descriptor fields; explanation requested | Asked “what do you mean limit and rules? simple example” | Needs concrete meanings for bounds and access permissions. Explain an ordinary bounded data segment with base 1000, limit 99, and read-only access; these are illustrative decimal values, not the repo configuration | Distinguish an in-bounds read, an in-bounds forbidden write, and an out-of-bounds access; independent application still pending |
| 2026-10-18 | M00 descriptor permissions; follow-up question | Said “oh ok” and asked “what kind of rules exist? read write delete?” | Assent is not independent evidence. Explain read/write/execute using actual x86 segment types, plus privilege; no delete permission for RAM. Stack descriptors are writable data descriptors, not a third hardware segment type | Apply the descriptor model to a short paper example after clarifying permission categories |
| 2026-10-18 | M00 GDT terminology; explanation acknowledged | Asked what GDT means, then said “got it” after “Global Descriptor Table” and its relation to segment-description records were explained | No independent answer assessed for the descriptor model yet; checkpoints remain unchanged | Predict a one-byte write using an illustrative descriptor with decimal base 2000, limit 99, and read/write data access; explain bounds and permission checks |
| 2026-10-18 | M00 descriptor paper application; correct for given access | Allowed the one-byte write at decimal 2050: “2050 < 2000 + 99” and “read and write allowed”; independently considered bounds and permission | Correct for this address. Explain that the last byte is included: base <= address <= base + limit; the strict inequality used here does not assess endpoint understanding. No runtime observation or full module completion claimed | Ask whether changing the byte's value from 42 to 99 requires changing the descriptor, and why |
| 2026-10-18 | M00 descriptor-versus-contents application; correct | Answered “no we're just changing the value” when asked whether replacing 42 with 99 requires changing the descriptor. Also correctly proposed that a write at 2099 is included | Description versus stored contents now independently distinguished in this example. Confirm inclusive endpoint for a one-byte write; a multi-byte access must fit entirely within the bounds. Full M00 scope/summary and later recall still pending | Ask for a revised one-sentence explanation of the GDT's role, replacing the earlier “store our memory” explanation |
| 2026-10-18 | M00 revised GDT explanation; unresolved | Proposed “gdt contains the instructions of our program? a set of rules, values, pointers” | Rules and starting-address intuition are useful, but GDT entries do not contain the program's executable instructions. Earlier value-update example remains valid evidence only for that distinction; no broad GDT-understanding checkpoint earned | Show a code-segment descriptor separately from the instruction bytes it describes; ask whether the table alone reveals the program's arithmetic |
| 2026-10-18 | M00 GDT-versus-instructions reasoning; correct core distinction | Correctly answered that the GDT alone cannot reveal addition versus multiplication: “no because it only contains memory segments, not the actual interaction”; followed with “so basically an adress, a value, a rule” | Core distinction between descriptions and executable operations demonstrated. Refine wording: it contains descriptions of segments, and “value” here should specifically name the limit, not a program variable's contents. Full module checkpoints still await remaining scope and summary evidence | Consolidate base address, limit, and access rules; continue remaining M00 scope rather than repeat obvious recognition checks |
| 2026-10-18 | M00 descriptor model; concise synthesis correct | Summarized “oh so just range + rule,” correctly reducing a segment descriptor to its described range and access rules | Core descriptor model now demonstrated through explanation and earlier paper application. Detailed hardware encoding is not needed at this stage; full M00 scope, summary, practice gaps, and later recall remain pending | Ask for a partner-facing explanation distinguishing the GDT from a stack dump; do not repeat descriptor recognition checks |
| 2026-10-18 | M00 GDT/stack comparison; partial explanation | Explained “gdt contains the memory ranges accessible, each range with a rule”; described stack dump as “the trace or history of the program” | GDT role is sound at introductory level. Stack dump shows current stack-memory contents, not a chronological execution log; saved return addresses can give clues about active calls, and stale bytes may remain without forming reliable history. M00-E/P remain pending | Explain stack reuse after a function returns, then ask why a dump cannot reliably reconstruct all earlier calls |
| 2026-10-18 | M00 GDT/stack distinction; corrected synthesis | Summarized “gdt: range + rules, stack dump: values at given time,” correctly distinguishing configuration descriptions from a snapshot. Also said “because it's not memory” | Correct the wording: these are values in stack memory; the dump is the snapshot. Earlier contents may be overwritten when stack space is reused, which is why a dump is not reliable program history. Full M00 checkpoints still await remaining scope and complete project explanation | Move beyond repeated descriptor checks to remaining M00 assignment constraints, bonus gate, and both partners' understanding |
| 2026-10-18 | M00 session wrap-up begun | Learner requests wrapping up today's introduction; no final recap assessed yet | Present remaining scope: 32-bit i386, GDT at 0x800 with six usable entries plus null, usable stack/readable dump, source/Makefile/bootable image under the subject's 10 MB limit, gated bonus shell, and both partners' ability to explain all modules | Learner gives a short own-words project summary and names one understood part and one remaining gap; assess checkpoints only from that answer, then save the next-session handoff |
| 2026-10-18 | M00 teamwork discussion; final recap still pending | Asked how to divide implementation work efficiently between partners | Planning advice is not evidence that both partners understand the modules; no checkpoints changed. Recommend shared boot/contracts, separate output versus GDT ownership, then dump versus packaging, followed by cross-teaching and joint verification | Return to the final own-words project summary when ready; both partners must ultimately explain all modules |
| 2026-10-18 | Partner plan saved; final recap still pending | Requested a Markdown record of the agreed work split for later; saved [PARTNER_SPLIT.md](PARTNER_SPLIT.md) with stages, shared interfaces, cross-teaching, and mandatory-before-bonus rules | Documentation maintenance only; no learning or runtime checkpoint earned | Resume the final M00 own-words summary; consult the partner plan when assigning implementation ownership |
| 2026-10-18 | M00 final recap assessed; session handoff | Explained compiling a kernel on our machine, bootloader loading, and running in QEMU; described GDT as an array of ranges/rules; described dump as program values at a given time. Explicitly identified forgotten subject scope and unknown “32-bit/i386” meaning | Build/boot model and descriptor model demonstrated. Clarify that the CPU uses the GDT and that the dump covers stack memory, not all program values. Mandatory/bonus and partner scope not independently recalled; no full E/Q/P/R checkpoint earned. No build or runtime verification performed | Next session: brief visual recall, host ARM64 versus target i386 and 32-bit meaning, compact subject map, then final explanation/scope checks before M01. Partner split is saved separately |

For a longer answer or transcript, add a dated note below this table and link to its heading from the row. Do not invent output or mark a command successful from a source comment.

### Keeping practice safe

- Paper exercises come first. Do not write to kernel physical addresses from a host program.
- Runtime labs happen in **QEMU**, not on your host machine's hardware.
- Before a temporary edit, preserve the exact original lines or patch. After observation, restore only your experiment, not unrelated changes.
- Do not deliberately corrupt the GDT, point ESP at arbitrary memory, overflow the stack, or enable unconfigured interrupts to “see what happens.” We can discuss those failures without creating them.
- Reboot and halt are emulator-only exercises. Their names are not permission to reboot or halt your workstation.
- A missing tool or failing build is a blocker to a lab, not proof that you failed the concept. Record the actual error and continue reachable paper work.

## Repository facts to keep straight

1. **The subject's “declare the GDT to the BIOS” wording:** this implementation loads the CPU's GDTR with `lgdt`. It is not a BIOS service call.
2. **User selector values:** this code defines `0x23`, `0x2B`, and `0x33`, including RPL 3. `0x20`, `0x28`, and `0x30` are their descriptor-index portions/byte offsets, not those full selector constants.
3. **Boot state:** Multiboot supplies usable flat segment state. It does not guarantee a valid inherited GDTR. Install our own GDT before reloading segment registers.
4. **First kernel code:** `_start` in assembly runs before `kmain`. `kmain` is the C entry point, not the kernel's first instruction.
5. **Dump context:** the dump captures registers inside `stack_dump`, after compiler-generated setup may have occurred. It is not just the return address of `call kmain`, and it is not a frozen backtrace.
6. **Current build prerequisite, found by source inspection:** `boot.asm` is at the repository root, but `Makefile` discovers assembly with `$(wildcard src/*.asm)`. The root boot object is therefore absent from that object list. Before a successful build/boot checkpoint can be claimed, that mismatch must be resolved. This workbook does not fix it or claim to have reproduced a build failure.
7. **Image evidence:** no `kfs2.iso` was present in the root listing when this workbook was prepared. Image creation, size, and ISO boot remain unverified, not completed deliverables.
8. **Hex-formatting caveat, found by source inspection while authoring the HTML workbook:** `src/printk.c`, `u32_to_hex_str`, fills eight positions with `hex_char(v & 0xFu)` without shifting `v`. For example, tracing the current source with `v = 0x2A` predicts `aaaaaaaa`, not the intended `2a`. Correct hexadecimal/endian calculations in lessons are conceptual values, not proof that this formatter displays them correctly. This finding has not been observed in QEMU or repaired; no learning or assignment checkpoint is earned by this inspection.

Some existing comments simplify or misstate details. We use them as teaching aids, then check the executable code and hardware/protocol contract. This workbook does not silently turn those comments into facts.

---

## M00 — What are we building?

### Explain it simply

In Linux, your program starts inside an already running operating system. Linux supplies services such as files, processes, and device drivers. Here, **our code is the kernel**: we cannot ask an existing Linux kernel to print text or give us a stack.

A bootloader gets our program into memory and hands over control. Our assembly entry code prepares a stack so C functions can run. Our C code prints text, constructs a GDT at the required address, tells the CPU to use it, and displays stack memory. The optional shell accepts a few debugging commands.

The mandatory part is small, but its pieces are tightly connected: no usable stack means ordinary C calls cannot work; no output means you cannot easily see progress; a malformed GDT can stop execution.

**Read:** [assignment](kfs2.pdf), chapters IV–VII; [overview](README.md), sections 1–4; [entry point](src/kmain.c), `kmain`.

### Explain / quiz targets

You should be able to state:

- The target is 32-bit i386, even though the host machine uses a different architecture.
- The GDT must be at `0x00000800` and describe kernel/user code, data, and stack.
- There are six requested usable descriptors plus the conventional reserved null slot.
- The stack must be displayed in a human-readable way.
- Source, a Makefile, and a basic bootable virtual image belong in the submission; the work must stay within the subject's 10 MB limit.
- Bonus features cannot compensate for a broken mandatory part.

**Questions:**

1. M00-Q1: How is this kernel different from a C program launched in a Linux terminal?
2. M00-Q2: Name the mandatory requirements without looking at the list.
3. M00-Q3: Which features are bonus, and when are they assessed?
4. M00-Q4: What would be wrong with “I only need to know my partner's half at a high level”?

### Practice

Give a two-minute project explanation to your partner or to me. Use ordinary words first; introduce “GDT” and “stack” only after explaining their roles. Identify one part you already understand and one part that currently feels like magic.

**Enough depth:** understand the goal and boundaries. No process scheduler, full Linux internals, filesystem, networking, or complete OS history required.

- [ ] M00-E — Explain the project and distinguish kernel from hosted program.
- [ ] M00-Q — Answer the scope questions in your own words.
- [ ] M00-P — Give the two-minute explanation and identify your gaps.
- [ ] M00-R — Repeat the project summary in a later session without notes.

## M01 — Bytes, addresses, registers, and the C we need

### Explain it simply

Memory is a sequence of byte-sized locations. An **address** tells you which location; the **contents** are the value stored there. A register is a small named storage location inside the CPU. `EAX`, `EBX`, `ESP`, `EBP`, and `EIP` hold 32-bit values in this execution mode, but have different conventional or architectural roles.

A byte is 8 bits; a 32-bit value occupies 4 bytes. Hexadecimal is just base-16 notation: one hex digit represents four bits. `0x800` and its decimal equivalent are the same number written differently. For sizes here, 1 KiB is 1024 bytes.

A C pointer is an address with a type telling the compiler how to access it. Dereferencing a pointer reads or writes the pointed-to object; the address itself is not the object's contents. Adding one to a `uint16_t *` advances by two bytes, not one.

The kernel uses fixed-width types such as `uint8_t`, `uint16_t`, and `uint32_t` because hardware layouts care about exact sizes. `const char *` is used for text; a C string ends with a zero byte (`'\0'`).

`volatile` on device memory tells the compiler that accesses matter even when ordinary C reasoning might treat them as unnecessary. It is **not** a lock, an atomicity guarantee, or a general memory barrier. `packed` prevents inserted structure padding; size assertions check that a hardware record has the required size. Neither validates the meaning of its contents.

`stack_bottom` and `stack_top` are externally defined address symbols. Their array declarations in C let us use their addresses; they are not variables holding the stack boundaries as stored integer values.

**Read:** [shared header](include/kernel.h), types, port helpers, packed structures, and stack declarations; [console](src/printk.c), `vga_mem` and `VGA_CELL`.

### Questions

1. M01-Q1: What is the difference between an address, a pointer value, and a byte stored at that address?
2. M01-Q2: How many bytes hold a 32-bit value? How many hex digits describe all its bits?
3. M01-Q3: Why does pointer arithmetic depend on the pointed-to type?
4. M01-Q4: Why is `uint32_t` useful in a descriptor-related structure?
5. M01-Q5: What do `volatile` and `packed` each do—and what do they not do?
6. M01-Q6: Why can we use compiler-provided types and headers without having a hosted `printf` or `malloc`?

### Practice

On paper:

- Convert `0x20`, `0x800`, and `0x100000` using hexadecimal place values. Show the method, not just a calculator result.
- Draw four successive byte addresses and store an example four-byte value in them.
- Draw a pointer variable separately from its pointed-to bytes.
- Trace a `uint8_t *` and a `uint16_t *` each advancing once.
- Point to the actual `stack_top` declaration and explain what its value means when used as a boundary.

**Enough depth:** pointers, byte sizes, strings, casts, bit shifts, masks, OR, and the specific qualifiers used here. We do not need a full C standards course. If these basics are shaky, spend time here before descriptor encoding.

- [ ] M01-E — Explain the essential memory and C vocabulary accurately.
- [ ] M01-Q — Answer the questions without confusing addresses and contents.
- [ ] M01-P — Complete and explain the memory/pointer worksheet.
- [ ] M01-R — Solve a fresh pointer/hex question later.

## M02 — Read the assembly, not an assembly textbook

### Explain it simply

An assembly instruction describes an operation directly. In NASM's Intel syntax, the operations used here usually read **destination, source**. Thus `mov esp, stack_top` puts the address named `stack_top` into ESP. Square brackets indicate a memory operand: `mov eax, [esp + 4]` reads a value from memory, rather than copying the arithmetic expression itself.

Registers have smaller named parts: AX is the low 16 bits of EAX; AL is its low byte. The assembly uses AX when loading 16-bit segment selectors.

Learn these instructions in context:

- `mov`: copy a value into a register or memory location.
- `push`: move ESP downward and store a value on the stack.
- `pop`: retrieve a stack value and move ESP upward.
- `call`: save the return address on the stack and transfer control.
- `ret`: obtain a return address from the stack and return there.
- `jmp`: transfer control; a far jump also supplies a code-segment selector.
- `cli`: disable maskable interrupts. It does not prevent all exceptions or NMIs.
- `hlt`: pause execution until a suitable wake-up event. A loop can halt again if execution resumes.
- `lgdt`: load the CPU's GDT-register information from a memory record.
- `in` / `out`: access x86 I/O ports, a separate address space from memory.

Assembler **directives** are not CPU instructions. `bits 32` selects instruction encoding; it does not switch CPU mode. `section` groups content; `global` exports a symbol; a label names an address; `dd` emits a 32-bit value; `resb` reserves bytes; alignment directives put the next content on the requested boundary.

Our `.asm` files use NASM/Intel syntax. GCC inline assembly in C uses AT&T syntax here, with registers written like `%esp` and source/destination order reversed. Do not apply one syntax's operand rules blindly to the other.

**Read:** [boot assembly](boot.asm), `_start`, `_hang`, and stack labels; [GDT assembly](src/gdt_flush.asm), `gdt_flush` through `ret`; [port helpers](include/kernel.h), `inb` and `outb`.

### Questions

1. M02-Q1: What is different about `mov eax, 0x800` and `mov eax, [0x800]`?
2. M02-Q2: What does `call` do that a simple `jmp` does not?
3. M02-Q3: Does `bits 32` put the CPU into protected mode? Who establishes that state here?
4. M02-Q4: Which lines reserve storage and which actually execute at runtime?
5. M02-Q5: Why use AX, rather than all of EAX, to load a segment selector?
6. M02-Q6: Why is “interrupts disabled” not the same as “nothing can fault”?

### Practice

Annotate a personal copy of `_start`: for each instruction, write what it reads, changes, and where execution goes next. Draw the stack before and after both argument pushes and `call kmain`. Explain the `gdt_flush` memory operand without yet decoding the descriptor bytes.

Do not edit the boot code for this exercise. If you want an extra challenge, explain a hypothetical `push eax; pop ebx` on paper, including its effect on ESP.

**Enough depth:** read every executable instruction in the two assembly files. No instruction-set memorization, opcode encoding, CPU pipeline theory, or assembly-only kernel rewrite required.

- [ ] M02-E — Explain the instructions, directives, and syntax distinctions used here.
- [ ] M02-Q — Answer the operand/control-flow questions correctly.
- [ ] M02-P — Produce the annotated instruction trace and stack drawing.
- [ ] M02-R — Trace a fresh short sequence without notes.

## M03 — How execution reaches C

### Explain it simply

For the ISO path, firmware starts the boot process and GRUB loads our kernel. The kernel follows **Multiboot version 1**, an agreement about file identification and CPU state at handoff.

The kernel file contains a header with three 32-bit words: magic `0x1BADB002`, flags, and a checksum. Their sum must be zero modulo `2^32`. The entire header must be four-byte aligned and within the first 8192 **file bytes**. That does not mean the header is at ELF file offset zero: the ELF has its own headers too.

At kernel entry, EAX contains the separate handoff magic `0x2BADB002`; EBX contains the address of a Multiboot information structure. Our kernel prints that address; it does not parse every field or use it to build a memory allocator.

The loader has already established 32-bit protected-mode execution with paging off and usable flat segment state. ESP is not a stack we can rely on. `_start` sets it to our `stack_top` before pushes and the C call. The inherited GDTR itself need not be valid; segment-register caches are why inherited memory accesses can still work before our own GDT is loaded.

Arguments follow the i386 cdecl convention: the last C argument is pushed first. `call` then adds a return address. The code that follows the call is a halt loop if `kmain` ever returns.

**Read:** [boot assembly](boot.asm), Multiboot header and `_start`; [linker script](linker.ld), `ENTRY(_start)` and `KEEP(*(.multiboot))`; [C entry](src/kmain.c), parameters and magic check.

### Questions

1. M03-Q1: Why do the header magic and the EAX handoff magic differ?
2. M03-Q2: What does EBX contain? What does it not contain?
3. M03-Q3: What must be true about the header's location, alignment, and checksum?
4. M03-Q4: At the instant `kmain` is entered, where are the return address and arguments relative to ESP?
5. M03-Q5: Why set ESP before the first push, rather than inside `kmain`?
6. M03-Q6: What can we rely on about inherited segments, and why should we not reload them through an assumed bootloader GDT?

### Practice

Draw the ISO boot timeline from firmware to `shell_run`. Mark the exact point where our code starts and where C starts. Compute the header checksum from the actual header words and explain 32-bit wraparound. Draw distinct pre-call and callee-entry stack layouts.

Then explain why the direct `-kernel` development route still needs a Multiboot header but does not prove the ISO's GRUB configuration works.

**Enough depth:** handoff contract, entry state, stack setup, and argument passing. No bootloader implementation or BIOS interrupt programming required.

- [ ] M03-E — Explain the boot handoff and what our entry stub establishes.
- [ ] M03-Q — Answer the header/state/argument questions without mixing moments.
- [ ] M03-P — Complete the timeline, checksum, and two stack-layout drawings.
- [ ] M03-R — Explain bootloader-to-C control flow later without notes.

## M04 — Compile, link, and boot the right artifact

### Explain it simply

The compiler translates C into machine-code object files. NASM translates assembly into compatible object files. Object files still contain unresolved references. The linker combines them, resolves symbols, and places sections according to **our own linker script**, producing `kernel.elf`.

The subject does not require writing a linker program: it requires controlling the layout with our own `.ld` script rather than using the host's default script. Here, `_start` is the entry, and the kernel image is linked at 1 MiB (`0x100000`). The GDT is separately constructed at `0x800` at runtime; do not assume every required location comes from the kernel's ordinary `.data` placement.

Sections are groups with different roles: `.text` for code, `.rodata` for read-only data, `.data` for initialized writable data, and `.bss` for zero-initialized storage. BSS occupies memory even when those zero bytes are not all stored literally in the file. The reserved stack is in BSS.

Flags you must explain:

- `-m32`: produce 32-bit code; `-march=i386`: restrict the instruction baseline.
- `-ffreestanding`: do not assume a hosted execution environment.
- `-fno-builtin`: do not treat ordinary named functions as hosted built-ins.
- `-fno-stack-protector`: avoid requiring a stack-protector runtime we have not supplied.
- `-nostdlib` / `-nodefaultlibs`: exclude normal startup/default library linking. The Makefile also invokes the cross-linker directly with only our objects.
- NASM `-f elf32`: create 32-bit ELF object files.
- Linker `-T linker.ld`: use our layout script.

The Makefile also includes `-fno-exceptions` and `-fno-rtti`; recognize their C++ purpose rather than claiming they add a useful C runtime feature.

`kernel.elf` and a bootable CD image are not the same artifact. `make run` uses QEMU's direct Multiboot-aware loading path. `make iso` stages the kernel and a GRUB configuration and uses containerized `grub-mkrescue`; `make run-iso` boots the CD image. Only the second route exercises the image's firmware/GRUB boot path.

**Read:** [Makefile](Makefile), variables, source discovery, pattern rules, link rule, ISO recipes; [linker script](linker.ld), output format, entry, sections, and symbols.

### Questions

1. M04-Q1: Explain source → object → ELF → ISO, naming each tool's role.
2. M04-Q2: Why can a kernel build on this host but not use its operating-system libraries at runtime?
3. M04-Q3: How does `.bss` consume RAM without equally increasing initialized file contents?
4. M04-Q4: Why does `-m32` alone not make a normal application into a working kernel?
5. M04-Q5: What does the link script control that the C compiler does not?
6. M04-Q6: What does `make run-iso` test that `make run` leaves untested?
7. M04-Q7: Using the current source discovery, is root `boot.asm` included? What evidence supports your answer?

### Practice

First do a paper dependency trace: list each source, expected object, link input, and generated artifact. Explain `$@`, `$<`, and the order-only `| build` prerequisite in a relevant Make rule. Find the recorded root-boot discovery mismatch before assuming the build can run.

**Runtime portion:** once the build prerequisite and toolchain are resolved, perform these existing Makefile targets yourself:

```sh
make
make run
```

For the deliverable route, with Docker available:

```sh
make iso
make run-iso
```

`make iso` depends on `img-docker`, so its recipe can build the container image; these commands may download software and create artifacts. We will check prerequisites before your lab. Learn how to close the QEMU window or quit through its monitor before an interactive run; do not assume every serial/monitor setup has the same escape keys.

Record actual errors or output, which artifact each command used, and which boot route it exercised. Do not tick P until the practical build and both boot-route observations have happened. If blocked, record that and continue the paper lessons.

**Enough depth:** narrate your build and identify the important flags, symbols, sections, and artifacts. No Docker internals, ELF relocation theory, or full GNU Make course required.

- [ ] M04-E — Explain the pipeline, freestanding flags, and linker layout.
- [ ] M04-Q — Answer the build/boot distinction questions and find the source mismatch.
- [ ] M04-P — Trace dependencies and personally build/observe direct and ISO boot paths.
- [ ] M04-R — Explain a fresh dependency or artifact question later.

## M05 — Printing without `printf`

### Explain it simply

No Linux terminal service exists inside this kernel. `printk` formats a small set of values and sends characters to two outputs: VGA text memory and the COM1 serial device.

VGA text mode here is 80 columns by 25 rows at `0xB8000`. Each cell occupies two bytes: character and color attribute. The code tracks a software row/column, handles newline/carriage return/backspace, scrolls when needed, and updates the hardware cursor. Writing a byte into an ordinary array is not the same as writing a character cell to this device memory.

COM1 uses I/O ports beginning at `0x3F8`, not a pointer into the VGA buffer. On first use, the code initializes the UART. Before transmitting a character, it polls whether the device is ready. Serial lets QEMU mirror kernel output into the host terminal even without a VGA window.

The formatter supports `%c`, `%s`, `%x`, `%d`, `%u`, and `%%`. `printk_hex` is a separate fixed-width zero-padded helper. The implementation walks the format string and retrieves the corresponding variadic arguments; wrong argument types/order are not automatically made safe.

Both output paths receive ordered characters, but they do not have identical representations: VGA stores cells and interprets control characters; serial sends bytes for a terminal to interpret. This implementation has **serial output, not serial shell input**.

**Read:** [output implementation](src/printk.c), `VGA_CELL`, cursor/scroll routines, `serial_init`, `serial_putc`, `console_putc`, `printk`, `printk_hex`; [public formatting contract](include/kernel.h).

### Questions

1. M05-Q1: Trace one character from a `printk` call to its VGA cell and serial output.
2. M05-Q2: Why two bytes per VGA cell? How is the cell index calculated?
3. M05-Q3: What happens at newline, the last column, and the last row?
4. M05-Q4: Why wait for UART transmit readiness? What does “polling” mean?
5. M05-Q5: Why can't you assume `%p`, `%08x`, or every hosted `printf` format works?
6. M05-Q6: Why would `volatile` not make all console operations atomic or thread-safe?

### Practice

Paper first: draw a VGA cell and trace a newline and one scrolling step. Walk through converting a small unsigned value into decimal or hex characters using the actual conversion code.

Then temporarily add a harmless line in `kmain` using only supported formats and correctly typed values. Predict the text and its destinations, rebuild, and observe it in QEMU. Restore the original line and record your observation and explanation. Do not change device addresses or I/O register configuration.

**Enough depth:** understand the path from format to visible output and the basic scrolling/conversion loops. You need not memorize every UART register bit or implement a terminal emulator.

- [ ] M05-E — Explain formatting, VGA cells, scrolling, and serial polling.
- [ ] M05-Q — Answer the output-contract questions accurately.
- [ ] M05-P — Complete the paper trace and observe your temporary output change.
- [ ] M05-R — Trace another output example later without notes.

## M06 — GDT: a table of rules, not a memory allocator

### Explain it simply

A segment descriptor tells the CPU about a segment: its base, accessible offset range, type, and privileges. A **selector** identifies which descriptor to use. CS selects code; DS and the other data segment registers support data accesses; SS selects the stack segment.

Think of the GDT as a table of descriptions, not a collection of actual code and data bytes. Creating an entry does not allocate RAM, move code, or create a stack. The actual kernel/stack storage comes from the linked image and reserved memory.

All six usable descriptors here are **flat**: base zero and an effective limit covering offsets through `0xFFFFFFFF`. They overlap in address range. This is not six disjoint memory allocations, nor evidence that four GiB of physical RAM exists. With paging off, the flat addresses used here reach physical addresses directly; paging would be a separate mechanism.

The table lives at `0x800`. Its location is different from a descriptor's base of zero. There are seven entries: a reserved null slot and six named descriptors. The stack descriptors are ordinary writable data descriptors used for stack access, not a separate special descriptor type.

A selector has an index, a table-choice bit (TI), and a requested privilege level (RPL). Here TI is zero, choosing the GDT:

```text
selector = (index << 3) | RPL
```

DPL is a privilege level in the descriptor; RPL is in the selector; CPL is the current execution privilege. The kernel runs at ring 0. Describing ring-3 segments or setting selector RPL bits does not, by itself, launch a user program or change CPL.

| Index | Role | Descriptor offset in table | Selector constant here |
|---:|---|---|---|
| 0 | Null slot | `0x00` | `0x00`, not a usable code/stack selector |
| 1 | Kernel code | `0x08` | `SEL_KCODE = 0x08` |
| 2 | Kernel data | `0x10` | `SEL_KDATA = 0x10` |
| 3 | Kernel stack | `0x18` | `SEL_KSTACK = 0x18` |
| 4 | User code | `0x20` | `SEL_UCODE = 0x23` |
| 5 | User data | `0x28` | `SEL_UDATA = 0x2B` |
| 6 | User stack | `0x30` | `SEL_USTACK = 0x33` |

**Read:** [header](include/kernel.h), GDT definitions/selectors; [GDT construction](src/gdt.c), table pointer and seven `set_gate` calls.

### Questions

1. M06-Q1: Distinguish table address, descriptor base, descriptor index, byte offset, and selector.
2. M06-Q2: Why seven slots when the subject requests six segments?
3. M06-Q3: Why can code, data, and stack descriptors all have base zero?
4. M06-Q4: Calculate the selectors from indices and RPL rather than reciting the table.
5. M06-Q5: What is different about DPL, RPL, and CPL? Does RPL 3 itself enter user mode?
6. M06-Q6: Does a “kernel stack descriptor” reserve or limit the actual stack to 16 KiB here?

### Practice

Hide the table above. Reconstruct the entry order, descriptor offsets, RPLs, and selector constants from memory and arithmetic. Decode the actual constants back into index, TI, and RPL. Separately draw the GDT at its table address and a segment whose base is zero; label why those bases differ.

**Enough depth:** explain flat segmentation, selector arithmetic, and this table's privileges. You need not design user processes, paging, LDTs, TSSs, or a ring-transition implementation.

- [ ] M06-E — Explain the GDT without treating it as an allocator or six disjoint regions.
- [ ] M06-Q — Answer selector/privilege questions with reasoning.
- [ ] M06-P — Complete the selector worksheet and table/base drawing.
- [ ] M06-R — Decode an unfamiliar selector later and explain the method.

## M07 — Descriptor bytes, GDTR, and switching to our table

### Explain it simply

The CPU expects an eight-byte layout, not whatever C structure layout happens to be convenient. Base and limit bits are split across fields. `packed` preserves that layout; `_Static_assert` checks the sizes of `gdt_entry` and `gdt_ptr`.

For a segment descriptor:

| Byte offsets | Meaning |
|---|---|
| 0–1 | Limit bits 15:0 |
| 2–3 | Base bits 15:0 |
| 4 | Base bits 23:16 |
| 5 | Access byte |
| 6, low nibble | Limit bits 19:16 |
| 6, high nibble | Flags |
| 7 | Base bits 31:24 |

`set_gate` uses shifts, masks, and OR to distribute the logical values into these fields. Understand the pattern rather than memorizing a string of bytes.

The access byte records presence, DPL, code/data versus system, executable status, direction/conforming behavior, readable/writable permission, and the accessed bit. Values supplied here are `0x9A` for kernel code, `0x92` for kernel data/stack, `0xFA` for user code, and `0xF2` for user data/stack. You should be able to identify the bits responsible for code versus data and ring 0 versus ring 3. The CPU can set the accessed bit when using descriptors; don't mistake that bit change for a different intended segment type.

The high flag nibble is `0xC`: G=1, D/B=1, L=0, AVL=0. G scales the limit in 4 KiB units. The effective last offset is `(limit << 12) | 0xFFF`; the configured 20-bit limit `0xFFFFF` therefore covers the full 32-bit offset range. D controls default operand/address size for this code descriptor; B selects 32-bit stack-pointer behavior for a stack descriptor. The access-byte direction bit is not what makes `push` decrement ESP.

**GDTR is not the GDT.** In this 32-bit code, `lgdt` reads a separate six-byte record: a two-byte table limit followed by a four-byte table base. Our seven eight-byte entries occupy 56 bytes, so the last valid table byte offset is 55. The record's base is `0x800`. The record itself can live in the linked kernel; only the actual table has the fixed-address requirement.

The C call passes `&gp` to `gdt_flush`. Assembly retrieves the pointer from `[esp + 4]` and executes `lgdt [eax]`. Passing the GDT table bytes instead would make the CPU misinterpret them as limit/base data.

The implementation then loads DS/ES/FS/GS with `0x10`, SS with `0x18`, and uses a far jump to reload CS with `0x08`. Loading GDTR alone does not refresh each segment register's cached descriptor information. An ordinary `mov` cannot load CS; the far jump performs the needed control transfer. Other architectural ways to load CS exist, but this routine uses a far jump.

Loading SS does not assign a new ESP or allocate a new stack. This routine keeps using the stack prepared by `_start`. User descriptors are defined but not loaded as part of a user-mode transition.

**Read:** [packed structures](include/kernel.h); [GDT construction](src/gdt.c), `set_gate`, assertions, `gp`, output, `gdt_flush` call; [loading routine](src/gdt_flush.asm), executable instructions.

### Questions

1. M07-Q1: Why do we need exact byte layout and size assertions? What do they not prove?
2. M07-Q2: Explain how a chosen base/limit is split and reassembled.
3. M07-Q3: Decode the important bits in a code access byte and a writable-data access byte.
4. M07-Q4: Why is the limit not simply a count of bytes when G=1?
5. M07-Q5: Draw the `lgdt` operand. Why is its limit 55 rather than 7 or 56?
6. M07-Q6: What exactly does EAX point to at `lgdt [eax]`?
7. M07-Q7: Why reload the segment registers? Why use a far jump for CS?
8. M07-Q8: Which part selects the stack segment, and which part holds the current stack offset?

### Practice

Paper first:

- Draw one descriptor's eight bytes and label each field.
- Trace one `set_gate` call through its assignments, including masks and flags/limit sharing byte 6.
- Draw the separate six-byte GDTR operand and the 56-byte table. Label where each lives and what the pointer argument targets.
- Trace the executable instructions from C call to `ret`, identifying exactly what state changes at each step.

Then, after the build lab is available, capture the GDT startup summary and the post-flush message. Explain their position in the code. The summary reads access/flag bytes before the flush; its selector list is constructed from constants. The final CS/DS/SS text also prints constants—it is not a hardware-register readback. Successful return and subsequent output are useful execution evidence, but describe that evidence accurately.

**Enough depth:** explain the values and sequence actually used, including the common wrong-operand failure. No memorization of every x86 descriptor type, deliberate triple-fault lab, or microarchitectural pipeline explanation required.

- [ ] M07-E — Explain descriptor layout, limit scaling, GDTR operand, and reload sequence.
- [ ] M07-Q — Answer the encoding/load questions without confusing table and operand.
- [ ] M07-P — Complete the diagrams/trace and interpret actual startup observations.
- [ ] M07-R — Reconstruct the load sequence and operand format later.

## M08 — The stack is memory, not a mysterious CPU box

### Explain it simply

The reserved kernel stack is 16 KiB in BSS. `stack_bottom` is its low address; `stack_top` is the exclusive high boundary, one byte past the storage. ESP starts at `stack_top` because pushing a 32-bit value first subtracts four, then stores it inside the region. Popping moves upward again.

ESP is the current stack pointer. SS supplies the stack segment description. EBP is often used as a frame pointer: a stable reference while ESP moves during a function. That use depends on generated code and compiler options; the register name alone does not prove a frame chain exists.

A call pushes the address of the following instruction so `ret` can return there. The function may also save registers, establish a frame, reserve locals, and make more calls. The generated stack layout is more than “our explicit pushes.” A local array can occupy stack storage; not every local necessarily does, because compilers can also keep values in registers.

The GDT stack descriptor is flat and does not enforce the reserved 16 KiB boundaries. Software must stay within that storage. A dump warning does not retroactively prevent overflow or restore overwritten memory.

**Read:** [stack reservation](boot.asm), stack labels and `_start`; [stack declarations](include/kernel.h); [dump](src/stack_dump.c), register snapshots; [call sites](src/kmain.c).

### Questions

1. M08-Q1: Why initialize ESP to an address just outside the high end of the storage?
2. M08-Q2: Starting from a chosen aligned ESP, what happens after two pushes, one call, one return, and two pops?
3. M08-Q3: How do SS, ESP, and EBP differ?
4. M08-Q4: What does a function prologue do? Why can't source-line order promise a snapshot before compiler setup?
5. M08-Q5: What would stack overflow mean here, and what actually enforces the bounds?
6. M08-Q6: Why might a dump taken from a shell command have a different call context from the boot-time dump?

### Practice

Choose an imaginary stack-top address. Draw two pushed arguments, the return address, a saved frame pointer, and hypothetical local storage. Label the boundaries and each ESP movement. Explain the difference between the guaranteed effects of `call`/`push` and compiler-dependent frame details.

Then identify the two actual places that must agree on stack size: the assembly reservation and `KERNEL_STACK_SIZE`. Do not change the size or force overflow.

**Enough depth:** explain direction, bounds, call/return mechanics, and ordinary frame concepts. No exploit development, complete unwinder, or compiler ABI edge-case survey required.

- [ ] M08-E — Explain stack storage, SS/ESP/EBP, frames, and bounds.
- [ ] M08-Q — Answer pointer-movement and call-context questions correctly.
- [ ] M08-P — Complete and explain the call-frame drawing.
- [ ] M08-R — Draw a fresh stack sequence later without notes.

## M09 — Understand what the stack dump shows

### Explain it simply

`stack_dump` records ESP and EBP from inside its own execution. Its generated frame can already exist. It uses the captured ESP to choose a starting row, rounding down to a 16-byte boundary, and uses `stack_top` as the exclusive end.

Each row shows an address, four 32-bit hexadecimal words covering 16 bytes, an ASCII view of those bytes, and ESP/EBP markers if their captured addresses fall in that row. The printable ASCII range is `0x20` through `0x7E`; other bytes become dots. Dots do not mean the underlying bytes are all zero.

x86 is little-endian: the lowest-addressed byte contributes the least-significant part of a multi-byte number. The word column assembles values using shifts, while the ASCII column follows increasing byte addresses. Both views describe the same memory, but the character order does not resemble the usual written order of a hexadecimal word.

The range is half-open: `[start, end)` includes start and excludes end. Rounding down means the first row may include bytes below the exact captured ESP. The output calls made after the snapshot can themselves use stack memory; this is a raw-memory display with captured pointer values, not an atomic frozen copy of the whole stack.

The routine caps output at 64 rows of 16 bytes: at most 1024 displayed bytes, not a 1024-byte stack. It reports truncation if rows are omitted. It warns and clamps its walking start if it is below `stack_bottom`; that is not a guard page, canary, or proof that a damaged stack can recover safely.

This is a **hexdump, not a backtrace**. It does not unwind calls or label every word as a typed C variable. An EBP marker appears only when the captured value lies inside a displayed row.

**Read:** [dump implementation](src/stack_dump.c), `ROW_BYTES`, `DUMP_MAX_ROWS`, register reads, alignment/clamp, word assembly, ASCII filter, marker tests, truncation message.

### Questions

1. M09-Q1: At what moment and in which function are the pointer values captured?
2. M09-Q2: Why round the start down, and which extra bytes might that include?
3. M09-Q3: What does `[start, end)` mean for the last legal byte read?
4. M09-Q4: Given four bytes in increasing-address order, how do you construct their printed word?
5. M09-Q5: Why might EBP have no visible marker? What does that not prove?
6. M09-Q6: How much data can be shown, and how do you recognize omitted rows?
7. M09-Q7: Why can't you identify every raw word's meaning or claim a complete backtrace?

### Practice

Paper first: use bytes `48 69 21 00` in increasing-address order. Derive their word value and ASCII view. Make a hypothetical 16-byte row containing both printable and nonprintable bytes. For a hypothetical pointer on a row boundary, explain which row's half-open test contains it.

Runtime: capture a real boot-time dump. Explain one row's address, words, ASCII, and any markers. Distinguish values actually observed from values expected from source. Do not guess a word is a return address just because it resembles a code address.

Optional safe experiment: temporarily reduce `DUMP_MAX_ROWS` to a smaller positive value. Predict whether the observed number of rows will actually trigger truncation, rebuild/run, and explain the result. Restore it. A smaller cap does not guarantee a truncation message if the dump was already shorter.

**Enough depth:** defend the range calculation, row rendering, endian interpretation, markers, and limits. No requirement to reconstruct every compiler-specific local or write an unwinder.

- [ ] M09-E — Explain the dump's presentation, snapshot context, and limitations.
- [ ] M09-Q — Answer byte-order/range/marker questions accurately.
- [ ] M09-P — Complete the row worksheet and interpret an actual captured dump.
- [ ] M09-R — Decode a fresh row and range-boundary example later.

## M10 — Put it together and reason about failures

### Explain it simply

The C sequence is first output → magic check → GDT installation → stack dump → bonus shell. The assembly stack setup happens earlier. Each milestone gives evidence about execution reaching that point, not a guarantee that everything afterward works.

If neither VGA nor serial shows the first banner, several hypotheses remain: the kernel may not have loaded, entry/stack setup may be wrong, or output may be broken. If VGA works but serial does not, investigate the serial device/capture path before changing the GDT. If output stops during the GDT transition, inspect the operand layout and selectors—but “it rebooted” alone is not a full diagnosis.

A triple fault is an exception-delivery failure escalating beyond a double fault; on the usual PC/QEMU setup it resets the CPU. A bad GDT can lead there, especially when there is no working exception-handling setup. Not every reboot is a triple fault, and not every triple fault has the same root cause.

This project keeps maskable interrupts disabled and polls keyboard input. Disabling interrupts does not prevent invalid-instruction or protection exceptions. We have not implemented the machinery for a general interrupt-driven kernel.

**Read:** [integration](src/kmain.c); calls into [GDT](src/gdt.c), [dump](src/stack_dump.c), and [shell](src/shell.c); [boot halt loop](boot.asm).

### Questions

1. M10-Q1: Explain the entire order and why the first print precedes our own GDT install.
2. M10-Q2: If the banner appears, which later requirements are still unproven?
3. M10-Q3: If output stops after the pre-flush GDT summary, which narrow facts and hypotheses can you state?
4. M10-Q4: How do you distinguish an observed symptom from a proposed cause?
5. M10-Q5: Why is blindly adding `sti` not a fix for a polling shell that appears idle?
6. M10-Q6: Why must the final ISO be tested separately from direct ELF loading?

### Practice

Create a milestone map with three columns: source operation, expected evidence if working, and actually observed evidence. Fill the observation column only from your own runs.

I will give you two hypothetical failure reports. For each, propose the next discriminating observation and relevant source region. Do not change code merely to guess. You do not need to produce or trigger a real fault to pass this reasoning exercise.

**Enough depth:** reason from observations, understand the usual GDT fault chain, and locate likely failure boundaries. No complete IDT/PIC driver or advanced debugger curriculum required.

- [ ] M10-E — Explain integration and what each milestone proves or leaves open.
- [ ] M10-Q — Answer diagnosis questions without turning hypotheses into facts.
- [ ] M10-P — Complete the milestone map and two hypothetical diagnosis exercises.
- [ ] M10-R — Handle a fresh failure scenario later.

## M11 — Mandatory mock defense and submission evidence

### Explain it simply

There are two different finish lines: **you understand the project**, and **the submitted project actually meets the requirements**. A good oral explanation cannot replace a bootable image, and a bootable image cannot prove that you understand its code.

Both partners should be able to explain every module. Cross-teach each other's code, but you do not need to rewrite the whole kernel to reach this defense-focused goal. Focus on closed-notes explanations and small, justified changes. One partner can ask while the other drives, then swap.

### Mock defense

Do this without lesson notes, with source available for code navigation rather than answer reading:

1. Give a five-minute boot-to-dump walkthrough.
2. Explain one instruction from each assembly file.
3. Calculate a selector from a descriptor index and RPL.
4. Draw a descriptor and the distinct GDTR operand.
5. Explain one `set_gate` assignment and one access/flag choice.
6. Explain why SS and ESP are different and why this stack has actual storage.
7. Decode one dump row and its boundaries.
8. Trace a character through `printk`.
9. Explain one build flag and one linker-script decision.
10. Handle a hypothetical failure without claiming an unobserved cause.
11. Make a small safe output change chosen by the examiner, predict it, build/run, explain the observed difference, then restore it.
12. State the important limitations honestly: no user program execution, no full backtrace, no serial shell input, no hardware 16 KiB stack bound from these flat descriptors.

If you can quote a constant but not explain its origin, revisit that module. If you understand the mechanism but forget a nonessential name, look it up and explain it. Understanding is the target, not perfect recitation.

### Assignment evidence — separate from learning boxes

All of these remain unchecked until observed or measured:

- [ ] A01 — Submitted source and Makefile are present; build-source discovery includes the actual boot stub.
- [ ] A02 — The resulting executable targets 32-bit i386 and uses our own linker script without host libraries.
- [ ] A03 — The Multiboot header is valid, aligned, and entirely within the required first 8192 file bytes.
- [ ] A04 — The GDT table is at `0x00000800` and contains the null slot plus the six required descriptors.
- [ ] A05 — GDTR loading and all intended segment reloads execute, and the kernel continues afterward. Distinguish printed constants from actual register inspection if stronger evidence is needed.
- [ ] A06 — The reserved stack and entry ESP setup agree, and the formatted dump runs with meaningful address/hex/ASCII information.
- [ ] A07 — The basic bootable image exists in the submission and boots through its GRUB/ISO path, not merely QEMU's direct ELF path.
- [ ] A08 — The submission's relevant size has been measured against the subject's 10 MB limit; do not mistake an ISO size alone for the whole submission's size.
- [ ] A09 — Both partners complete a mandatory mock defense and can navigate each other's code.

For each A item, log the command/inspection/observation and actual result. We will choose the relevant artifact inspection tools when performing the lab rather than treating source comments as proof.

### Completion rule

**Mandatory defense-ready:** all M00–M11 E/Q/P/R boxes complete, all A items validated, and no known mandatory blocker left unexplained or unresolved. Timing estimates do not override this rule. Bonus work starts only after this gate.

- [ ] M11-E — Give a coherent whole-project explanation and identify its limitations.
- [ ] M11-Q — Pass a mock defense with follow-up questions from both partners.
- [ ] M11-P — Perform the safe change exercise and collect the assignment evidence.
- [ ] M11-R — Repeat the weak points in a later closed-notes mock defense.

---

## B01 — Bonus: where keyboard characters come from

### Explain it simply

A PS/2 keyboard sends scancodes, not C characters or ASCII strings. This kernel polls the controller status at port `0x64`; when output is ready, it reads a byte from port `0x60`. A table translates supported set-1 codes into characters.

A make code normally represents a press; a break code represents release. Releases usually do not become shell characters. Shift changes how a supported key is translated. The implementation keeps a simple shared Shift state, not a complete keyboard model with independent modifier tracking and every extended key.

Input is from the emulated PS/2 keyboard. COM1 output in your terminal does not mean typing into that terminal feeds this shell. Use the QEMU keyboard/window for an interactive lab.

The supported layout is a narrow US-style subset. This is not a complete driver: Caps Lock, extended key handling, and some modifier sequences are not fully implemented. An introductory comment calling scancode `0x1E` “Q” conflicts with the actual table mapping it to `a`; use the executable table and a verified scancode reference rather than memorizing that prose.

**Read:** [keyboard implementation](src/keyboard.c), `sc2c`, `keyboard_getchar`, status/data reads and Shift handling.

### Questions

1. B01-Q1: What does each of ports `0x64` and `0x60` provide here?
2. B01-Q2: What is polling, and how does it differ from interrupt-driven input?
3. B01-Q3: How does a scancode become a character? What happens to unsupported codes?
4. B01-Q4: What state changes on Shift press/release?
5. B01-Q5: What can go wrong when both Shift keys are held and just one is released in this implementation?
6. B01-Q6: Why doesn't serial output establish a serial input path?

### Practice

Paper-trace one supported letter, its release, and that letter while Shift is held. Show the status/read, code, modifier state, and return value. Then trace the two-Shift sequence against the actual implementation and explain its limitation.

Runtime, after the mandatory gate: use QEMU's keyboard to type supported characters and a shell command. Record expected versus observed case/translation. Do not enable interrupts or reconfigure the controller as an experiment.

**Enough depth:** explain this polling/translation path and its limitations. No complete PS/2 command protocol, PIC/IDT setup, or international keyboard driver required.

- [ ] B01-E — Explain scancodes, polling, translation, and the supported subset.
- [ ] B01-Q — Answer the input/state questions and identify current limitations.
- [ ] B01-P — Complete the state traces and observe emulator keyboard input.
- [ ] B01-R — Trace a fresh key/modifier sequence later.

## B02 — Bonus: a debug loop, not a POSIX shell

### Explain it simply

`shell_run` reads translated characters, echoes them, stores a command line, handles backspace/Enter, adds a NUL terminator, and compares the resulting string with supported command names. It is an endless loop in the kernel, not a separate Linux process.

The static line buffer has 128 bytes: at most 127 stored characters plus the terminator. Current code echoes additional characters even when they are not stored. Thus visible input and the stored command can diverge at capacity; “has a bound” does not mean “handles excess input perfectly.” Backspace behavior also depends on the console's row handling.

Commands are `help`, `printstack`, `reboot`, and `halt`. `printstack` calls the same dump from a deeper call path. `reboot` requests controller reset with `0xFE` to port `0x64`, then waits in a halt loop if reset does not take immediately. `halt` disables maskable interrupts and repeatedly executes `hlt`; it is not an ACPI power-off request.

**Read:** [shell implementation](src/shell.c), buffer constants, `str_eq`, command handlers and `shell_run`; [console](src/printk.c), backspace/newline handling.

### Questions

1. B02-Q1: Trace a line from keyboard input to command dispatch.
2. B02-Q2: Why reserve one byte for the terminator? What happens at capacity in this code?
3. B02-Q3: Does this shell support processes, pipes, files, or command arguments?
4. B02-Q4: Why does `printstack` not necessarily repeat the boot-time byte contents?
5. B02-Q5: What exactly do reboot and halt request? Why is halt not power-off?
6. B02-Q6: What happens after an empty or unknown command according to the actual dispatch code?

### Practice

Paper-trace `help`, a misspelled command, a line with backspace, and input at buffer capacity. Track both the stored string and visible output. Inspect string comparison on equal strings and when one is a prefix of the other.

In QEMU only, after the mandatory gate, try `help` and `printstack`; explain actual output. Observe reboot and halt in disposable emulator sessions, with a known way to quit/relaunch QEMU. Record whether reboot actually restarts the kernel and what halt visibly does; do not equate silence alone with proof of a hardware state.

Optional safe edit: change a help string or add a command that only prints a fixed message. Predict, run, explain, and restore. No new hardware-control command needed.

**Enough depth:** line storage, exact string dispatch, loop behavior, and actual reset/halt mechanisms. No POSIX shell, terminal subsystem, scheduler, or ACPI implementation required.

- [ ] B02-E — Explain the shell flow, storage limits, and command mechanisms.
- [ ] B02-Q — Answer the command/buffer questions with the code's actual behavior.
- [ ] B02-P — Complete the traces and emulator-only command observations.
- [ ] B02-R — Explain a fresh shell input/control-flow example later.

## Small glossary — revisit only when needed

- **ABI:** the agreement about how compiled code passes arguments, uses registers, and returns values.
- **BSS:** memory for zero-initialized storage, without storing all initial zero bytes literally in the executable.
- **Descriptor:** a record describing a segment's base, limit, type, and privileges.
- **ELF:** the executable/object file format used by this build.
- **Frame:** one function's compiler-created stack arrangement; not every build uses an EBP-linked frame.
- **Freestanding:** code that does not assume a hosted program environment or its normal library/startup services.
- **GDTR:** CPU register information identifying the GDT's base and table-byte limit.
- **Granularity:** the scale used to interpret a descriptor's limit.
- **I/O port:** an x86 device-access address used with `in`/`out`, separate from ordinary memory addresses.
- **Linker symbol:** a named address/value resolved by the linker, such as an entry or boundary symbol.
- **MMIO:** memory-mapped I/O; device access through memory addresses, as with VGA text storage.
- **Multiboot:** the kernel/loader handoff protocol used here; this kernel uses version 1.
- **Polling:** repeatedly checking a status instead of waiting for an interrupt handler to notify you.
- **Protected mode:** the x86 execution mode used here, with descriptor-based segmentation and privilege checks.
- **Selector:** index/table-choice/requested-privilege bits identifying a segment descriptor.
- **UART:** the serial device used for COM1 output.

## References and stopping boundary

Use the repository source first for “what our code does,” and the assignment for “what we must deliver.” For “what the hardware/boot contract means,” use:

- [GNU Multiboot specification — machine state](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state), also the OS image format section for the header rules.
- [Intel Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html), Volume 3 segmentation material when we need to check a descriptor rule.
- The curated OSDev links already collected in [README.md](README.md), as supporting tutorials rather than an instruction to read everything.

Do not start studying scheduling, paging implementation, filesystems, networking, multicore synchronization, or every x86 exception simply because those topics are nearby. We will explain adjacent concepts only as far as needed to defend this code. Expand the scope only if you want deeper study after the mandatory gate.

## Start here

**First explanation:** a normal Linux program runs on top of an operating system that is already doing the hard setup. Our kernel does not. A bootloader loads it, our entry code prepares what C needs, and our code directly configures hardware and writes output. That is why familiar calls like `printf` are not automatically available.

**First question (M00-Q1):** in your own words, what does Linux normally provide to a running C program that this kernel must either provide itself or do without? Name two concrete examples. Answer first; then we will correct or extend it and move to the next small step. All progress boxes are still unchecked.
