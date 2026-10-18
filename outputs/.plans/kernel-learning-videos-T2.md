# T2: Discover and assess beginner teaching quality

## Approved task
The user approved deep research on 2026-10-18. They want VIDEO, not written substitutes, to understand writing a minimal C kernel that an existing bootloader loads. Target: 32-bit i386, GRUB/Multiboot1, QEMU. Host is macOS ARM64, but conceptual lessons may use Linux if the setup difference is flagged. They know almost no assembly, are rusty at C, learn visually, and find long noisy explanations overwhelming. They want active tutoring and to write all exercises themselves.

Read the overall plan: outputs/.plans/kernel-learning-videos.md.

## Your slice
Independently discover approachable video lessons and assess whether the teaching actually explains the C-to-boot pipeline, rather than assuming all prerequisites. Seek overlooked creators, course lectures, small OSDev/Bare Bones walkthroughs, and bounded episode sequences. Prioritize small explained kernels, not months-long custom bootloader development or large advanced OS projects.

For each credible candidate, inspect primary description/transcript/chapters and linked creator code where useful. Assess: does the teacher define registers, stack setup, entry points, object files and linking when introduced? Is it a step-by-step explanation or live debugging with assumed knowledge? Can a short segment stand alone? Use evidence; do not equate popularity with quality or claim visual inspection without visual evidence.

Do not recommend generic CPU circuitry videos. Already considered: Viktor Engelmann (C++, Linux host GRUB changes, assumes assembly), CodePulse (64-bit/Multiboot2), and Queso Fuego/amateuros (homemade bootsector/custom loader). Treat these as rejected/partial baselines, not new exact matches. offlinemark's 'realistic OS/kernel dev session in C (week 69, grub/multiboot)' starts from JOS and assumes paging/GDB; it is too advanced.

## Evidence and constraints
Verify direct video links, title/channel, duration if available, actual language, architecture, bootloader/protocol, prerequisites, and beginner fit. Mark unknown facts, inferred fit, visual limitations, and host/toolchain mismatches. No invented timestamps. No PDF parsing, repo edits outside your output, builds, tests, formatters, or installations.

## Output
Write outputs/.drafts/kernel-learning-videos-research-T2.md. Include:
- At most 4 credible candidates or an honest no-strong-match finding.
- A practical teaching-quality assessment with transcript/primary-source evidence.
- A compact search and rejection log.
- Exact direct URLs, evidence availability, caveats, and any access/tool failures.

Do not draft the final ranked answer. Return the file path and a short summary; the lead owns the synthesis.
