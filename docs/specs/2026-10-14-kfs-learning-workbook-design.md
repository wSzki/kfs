# KFS personal learning workbook

## Selected approach

The learner approved a guided workbook. Each module has a lesson, questions, a paper exercise, and defense prompts.

## Purpose and boundaries

- Teach this repository's mandatory KFS_2 material to a learner comfortable with Linux but new to assembly and kernels.
- Start at M00, emphasizing the difference between a library function such as printf and the operating-system/device support it needs.
- Follow CHECKLIST.md's M00–M11 sequence. Include B01/B02 in a clearly separated bonus appendix with the mandatory-first gate explained. No claims that browser activity satisfies that gate.
- Support solution-first learning with actual small code excerpts, then reasoning, recall and paper traces.
- No kernel changes, host hardware experiments, deliberate fault labs, deployment, or changes to assessed learning progress.
- Use the online resources named in README.md and CHECKLIST.md as teaching sources, not merely as a link list: OSDev Bare Bones, Meaty Skeleton, GDT Tutorial, Multiboot, VGA, Serial Ports and PS/2; the official Multiboot v1 specification; Intel SDM; NASM/GNU linker references where relevant.
- Read and verify the relevant primary pages before writing their lessons. Add per-module source links and brief source-to-code connections. Write original explanations and questions; do not reproduce long copyrighted passages or the assignment PDF.
- Distinguish architecture/protocol guarantees, this repository's implementation, conceptual examples and actual runtime observations.

## Deliverable

A single self-contained `learn.html` at the repository root. Embedded CSS, JavaScript and course content; no external scripts, fonts, images, packages or network calls. Opens directly in a browser and can also be served over localhost. English content, responsive layout, printable lessons and worksheets. README.md gains a short entry point explaining how to open it and its progress limitations.

## Visual direction

Use a personal study notebook layout. Use warm paper tones, charcoal text, green accents, thin rules, and clear type.
Keep the course index beside the lesson on desktop. Use a disclosure for the index on mobile.
Use a compact title, not a marketing hero. Start the lesson near the top of the page.
Remove account, telemetry, privacy, assessment, and normal saving-status boilerplate from the page.
Show short warnings only when a problem needs action.
Use STE-flavored prose throughout the lessons. Use short sentences, active voice, common words, and one name for each concept.
Keep visible keyboard focus and readable contrast. Support reduced-motion settings.

## Course depth

All 12 mandatory modules and two bonus modules have:

1. A plain-language objective and definitions of new vocabulary.
2. Several short explanations focused on why the code works, not just constants to memorize.
3. A small annotated code excerpt or concrete memory/byte example with a source-file and symbol reference.
4. At least four original single-answer multiple-choice questions with plausible misconception-driven alternatives. More questions for GDT encoding/loading and the stack where needed.
5. Explanation of the correct answer and why each alternative fails, shown only after an attempt.
6. A paper-first exercise with an editable personal answer area and a deliberately revealed worked solution.
7. An oral-defense question and partner cross-teaching prompt; no automated grading of free-text reasoning.
8. A recap and clear enough-depth boundary to avoid unrelated kernel topics.

Include a searchable glossary and a review of missed questions. Keep answer choices stable.
A question belongs in the review if any attempt was incorrect. Keep the first-attempt result when the learner tries again.
Explain the difference between source inspection and observed output where the lesson needs that distinction.

## Conceptual interactions

Include three small explanatory tools: selector decoding/calculation (index, TI, RPL), a downward-growing stack push/call/return paper trace, and little-endian word/ASCII decoding. Label them conceptual models, not emulation or runtime observations. Each has an accessible textual explanation and safe bounded inputs. Diagrams illustrate relationships; tables compare numeric values. Keep these tools subordinate to lessons.

## State and privacy

- Store answer attempts, current lesson/question and personal notes locally in browser storage when available.
- If saving fails, show a short warning. Keep the page usable without storage.
- Show no saving-status banner when storage works.
- Save the current module and question. Explain file-location and browser-storage limits in README.md, not in page banners.
- Provide export of personal progress/notes for backup or a tutoring session and an explicit confirmed reset.
- Personal answers stay out of the HTML source and Git; no telemetry, accounts, remote APIs or automatic CHECKLIST.md updates.
- Show first-attempt results and later attempts. Keep assessed progress in CHECKLIST.md separate from workbook answers.

## Grounded facts and pitfalls

CHECKLIST.md is the learning source of truth; executable code takes precedence over misleading source comments.

- `_start` is the assembly entry before C's `kmain`.
- Multiboot v1 header magic differs from the EAX handoff magic. EBX is the information pointer; entry prepares its own stack.
- Seven GDT entries = null slot plus six usable descriptors. Table address is 0x800; descriptor bases are zero.
- Kernel selectors are 0x08/0x10/0x18; user selector constants are 0x23/0x2B/0x33, not merely offsets 0x20/0x28/0x30.
- GDT entries are 8 bytes. The distinct packed GDTR operand is 6 bytes, with limit 55 and table base 0x800. `lgdt` alone does not reload cached segment descriptors.
- Flat descriptors do not reserve memory, guarantee four GiB of physical RAM, enforce the stack's 16 KiB reservation or enter user mode.
- `gdt_flush` uses a far jump for CS, not the only possible architectural method.
- Stack top is the exclusive high boundary. ESP/EBP are captured inside stack_dump, not at entry to kmain; the dump is not a complete backtrace.
- Output goes to VGA and COM1; serial output does not supply this shell's keyboard input.
- Printed selector constants are not hardware-register readback.
- Source inspection finds Makefile's src/*.asm discovery omits root boot.asm; no build failure has been reproduced in this work. ISO presence/boot remain unverified.
- Bonus keyboard scancode table maps 0x1E to a despite conflicting comments; its single Shift flag cannot correctly retain two simultaneous held Shift keys through either release.

## Verification and acceptance

Run the actual page in Chromium via a temporary localhost server. Exercise lesson navigation, incorrect/correct QCM feedback, question advancement, missed-question review, conceptual tools, notes, persistence after reload, export and confirmed reset. Check blocked/corrupt storage behavior, keyboard operation, desktop/mobile overflow, browser console errors and print usability. Inspect desktop and mobile screenshots. Verify the final self-contained page makes no external requests and can load offline. Review course content against source anchors and preserve assessment/bonus distinctions. Do not build the kernel as part of verifying this HTML-only change.

## Implementation shape

Split content authoring into foundations M00–M05 and core/defense M06–M11 plus bonus; a separate integration owner builds the single HTML interface and embeds both content sets. Content is plain structured data with stable module/question IDs and per-choice rationale. Only the integration owner edits learn.html and README.md. Temporary content files and verification artifacts stay outside the repository and are not deliverables.
