# T1: Find exact-topic kernel video walkthroughs

## Approved task
The user approved deep research on 2026-10-18. Find video resources for a visually oriented beginner building a minimal 32-bit i386 C kernel loaded by GRUB/Multiboot1 and run in QEMU. The user knows almost no assembly, is rusty at C, and is overwhelmed by information-dense teaching. The important pipeline is kernel.c -> object file, tiny assembly entry/stack, linker script, ELF kernel, bootloader handoff and first visible output.

Read the overall plan: outputs/.plans/kernel-learning-videos.md.

## Your research slice
Discover and verify exact-topic videos. Look beyond generic CPU material and already-suggested mismatches. Useful leads include CodeTutorials / 'Writing a Basic Kernel' / 'Writing a Basic Operating System', OSDev Bare Bones video adaptations, littleosbook-related videos, smaller kernel-development creators, public university tutorials, and videos hosted outside YouTube if needed. These leads are not recommendations until verified.

Prioritize measured explanations of C + small x86 assembly boot stub + GRUB/Multiboot + linker/build. Don't treat Linux kernel modules, BIOS-only boot sectors, writing a custom bootloader, or x86-64 as exact matches. If an otherwise excellent video differs, label the difference explicitly rather than hiding it.

Previously suggested and inadequate as main choices: In One Lesson/Branch Education general CPU videos; Viktor Engelmann's 'Write your own Operating System in 1 hour' (C++, old Linux host commands, assumes assembly); CodePulse 'Write Your Own 64-bit Operating System Kernel #1' (64-bit destination/Multiboot2, first episode assembly-only). Don't repeat these as newly discovered exact matches.

## Evidence and constraints
Use web search and read primary video pages/descriptions/transcripts and creator source repositories. Verify exact URLs, title, channel, duration if available, architecture, language, boot protocol, topics and stated prerequisites. Do not invent timestamps or claim to have visually watched a video if only its transcript was available. A transcript establishes spoken coverage, not visual quality. Distinguish verified facts and your pedagogical inference. No PDF parsing. No code changes, builds, tests, installs, or unrelated repository inspection.

## Output
Write outputs/.drafts/kernel-learning-videos-research-T1.md. Include:
- At most 4 plausible finalists, or an honest statement that no strong exact match was found.
- Direct video URLs and primary evidence for the above facts.
- Exact episode(s) to watch and why they fit this learner; whether they explain unfamiliar syntax or assume it.
- A compact search log and rejection list showing where you looked and why similar-but-wrong videos were rejected.
- Any tool/access failures and unresolved claims.

Do not write final recommendations or a polished report. Return the file path and a short summary. The lead ranks and synthesizes.
