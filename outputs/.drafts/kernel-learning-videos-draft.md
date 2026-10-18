# Kernel-learning videos: a focused shortlist

**Research date: 2026-10-18.** Your goal is a small **32-bit C kernel loaded through Multiboot1**, not a general computer-hardware course or a new bootloader project.

## Bottom line

I found closer matches than the earlier suggestions, but **did not verify a great standalone course that both follows your exact setup and teaches everything from zero assembly**. The evidence supports one conditional, exact-topic viewing choice and one optional assembly-basics bridge—not a padded list of supposedly perfect tutorials.

## 1. Closest to the exercise: sphaerophoria

**[Learning (and inferring) about OS bootstrapping](https://www.youtube.com/watch?v=gBykJMqDqH0)** — **52:04**, published 2023-09-05.

This recording follows OSDev Bare Bones: a C kernel, a small x86 assembly entry, stack setup, Multiboot1, linker sections, building, and QEMU. It discusses the same object-file/section questions we reached in your lab, rather than unrelated CPU circuitry.

**Where I would start for you [INFERENCE]:** sample the **25:00 C-kernel chapter** first. The next creator chapter starts at **28:13**, so you can stop there instead of taking on the whole stream. If the C part makes sense, the **05:12 bootstrapper/Multiboot chapter** is the relevant next topic. These are creator-supplied chapter starts, not frame-checked boundaries.

**Why this is conditional:** it is a think-aloud learning stream, not a polished introductory class. The speaker looks things up, makes guesses, and spends time debugging Linux/NixOS setup. Some linker/entry explanations remain speculative. It is suitable to discuss together in small excerpts, **not an authoritative recipe to copy or proof that you should already understand assembly**. The ending also changes the boot test approach; the transcript alone does not certify that the final successful run uses the rebuilt GRUB ISO rather than direct Multiboot loading.

## 2. For the unfamiliar assembly words: nanobyte

**[Building an OS — 1 — Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc)** — **23:18**, published 2019-01-26.

This is the stronger evidence-backed vocabulary bridge: it explicitly introduces **assembly, instructions versus directives, registers, addresses versus contents, and the stack** before using them in a small bootable program. It explains the difference between loading a label's address and reading the memory at that address—the exact distinction that was causing trouble in `kernel.c` and the disassembly. The creator also supplies diagrams and an almost-verbatim transcript.

**The boundary is important:** this episode builds a **16-bit BIOS bootsector**, not your 32-bit C/GRUB kernel. Use it only to make assembly vocabulary less mysterious. Do **not** transplant its `org 0x7C00`, 16-bit segment calculations, BIOS interrupts, or floppy-image build into your lab. It does not replace the kernel walkthrough above.

The creator transcript contains small clarity edits. Auto captions disagree with it on `mov` direction and stack ordering; neither captions nor any tutorial should override the actual instruction examples. For reference, the intended NASM `mov` form here is destination on the left, source on the right; a stack is last-in-first-out. No short segment timestamp was independently verified.

## What this search did not establish

- No verified zero-prerequisite, exact-topic video deserves an unqualified “best for you” claim.
- Spoken coverage was assessed through captions/descriptions and creator material, **not a full audiovisual viewing**. Screen readability, delivery and actual pace remain unverified; teaching-fit judgments are inferences.
- None of the selected material is a complete macOS ARM64 setup guide. Keep the project's existing cross-toolchain and build constraints.

## Why the other candidates did not make the shortlist

- **Pritam Zope — [Create Your Own Operating System (OS) - Kernel](https://www.youtube.com/watch?v=4hJDOvwbTZs), 59:54:** a much closer C/GRUB/QEMU topic match, supported by the creator description and companion source. However, captions were unavailable, so spoken explanations and pacing could not be checked. It remains an **unrated option**, not a confident beginner recommendation. Its old Linux/GNU-AS scripts also differ from newer NASM examples in the repository.
- **Ubuntu OnAir — Do You Want To Build a Kernel?:** some useful beginner questions and a later C/GRUB core, but a long bootsector detour, live C-call confusion, and no ESP initialization in the video-linked episode-one startup source make it unsuitable as your first recipe. This is a source-level finding, not an observed test failure.
- **Ricky Valentine Oswald's Bare Bones recordings:** insufficient captions/content evidence to assess teaching.
- **CodePulse:** still x86-64/Multiboot2; **Viktor Engelmann:** still C++ and assumes assembly. Neither becomes an exact match merely because a broad search ranks it highly.
- **General CPU animations, Linux administration/module lessons, and long homemade-loader series:** wrong task or too much prerequisite detour.

## Evidence notes

Primary content and metadata were checked for the linked videos. Technical topic matching was cross-checked against creator transcripts/source where available. The detailed candidate/rejection evidence is in the research artifacts; citations and independent verification are the next step for this draft.
