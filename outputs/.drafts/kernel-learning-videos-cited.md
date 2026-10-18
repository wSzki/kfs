# Kernel-learning videos: a focused shortlist

**Research date: 2026-10-18.** Your goal is a small **32-bit C kernel loaded through Multiboot1**, not a general computer-hardware course or a new bootloader project.

## Bottom line

**This search did not establish a strong standalone course that both follows your exact setup and teaches everything from zero assembly.** That is a limit of the candidates and evidence assessed, not a claim that no such video exists.[16, 17, 18] My recommendation is one conditional, closely matching viewing choice and one **optional** assembly-vocabulary bridge—not two supposedly perfect tutorials.

## 1. Closest to the exercise: sphaerophoria

**[Learning (and inferring) about OS bootstrapping](https://www.youtube.com/watch?v=gBykJMqDqH0)** — **52:04**, published 2023-09-05.[1]

This recording follows OSDev Bare Bones: a C kernel, a small x86 assembly entry, stack setup, Multiboot1, object files/linker sections, building, and QEMU.[1] Those are the relevant topics; that does **not** establish an unambiguous end-to-end GRUB demonstration.

**Where I would start for you [INFERENCE]:** sample the **25:00 C-kernel chapter** first. The next creator chapter starts at **28:13**, so you can stop there instead of taking on the whole stream. If that makes sense, the **05:12 bootstrapper/Multiboot chapter** is the relevant next topic. These are creator-supplied chapter starts, not frame-checked boundaries.[1]

**Why this is conditional:** it is an exploratory learning stream with admitted guesses, unresolved linker/entry explanations, and Linux/NixOS setup debugging.[1] I would use it for discussion in small excerpts, **not as an authoritative recipe or proof that you should already understand assembly** [INFERENCE]. Near the end, the speaker says they can “get away without setting up grub”; the transcript alone does not establish that the final successful run uses a rebuilt GRUB ISO rather than direct Multiboot loading.[1]

## 2. For the unfamiliar assembly words: nanobyte

**[Building an OS — 1 — Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc)** — **23:18**, published 2019-01-26.[2]

**Optional vocabulary bridge [INFERENCE]:** it explicitly introduces **assembly, instructions versus directives, registers, addresses versus contents, and the stack**.[2, 3] In particular, it contrasts loading a label's address with reading memory at that address. The creator's written transcript includes register and memory-segment diagrams and describes itself as **almost verbatim, with minor clarity changes**—not an exact record of the original speech.[3] This does not establish the video's visual readability.

**The boundary is important:** this episode builds an **assembly-only, 16-bit BIOS bootsector**, not your 32-bit C/GRUB kernel; C is deferred to later work.[2, 3] Use it only to make assembly vocabulary less mysterious. Do **not** transplant its `org 0x7C00`, real-mode segment calculations, BIOS interrupts, or floppy-image build into your lab: those belong to its different boot path.[3] It does not replace the kernel walkthrough above.

The auto captions and creator transcript disagree on `mov` direction and stack ordering.[2, 3] The creator transcript gives the intended NASM form as destination on the left, source on the right, and describes first-in-last-out stack access.[3] Treat this as a reason to pause and check meaning, not to copy code uncritically. **No short segment timestamp was established.**[18]

## What this search did not establish

- No zero-prerequisite, exact-topic video earned an unqualified “best for you” recommendation in this research.[16, 17, 18]
- Coverage was assessed through captions, descriptions, and creator material, **not a full audiovisual viewing**. Screen readability, delivery, and actual pace remain unassessed; teaching-fit judgments are inferences.[16, 17, 18]
- The selected recordings use Linux/NixOS or Ubuntu; neither demonstrates a complete macOS ARM64 setup.[1, 3] Keep the project's existing cross-toolchain and build constraints.

## Why the other candidates did not make the shortlist

- **Pritam Zope — [Create Your Own Operating System (OS) - Kernel](https://www.youtube.com/watch?v=4hJDOvwbTZs), 59:54:** the creator description and linked companion establish a close C/GRUB/QEMU topic match.[4, 5] Captions were unavailable to the reader, so spoken explanations and pacing could not be assessed.[4] It remains an **unrated option**, not a confident beginner recommendation. Its Linux/GNU-AS/script workflow also differs from the repository's newer NASM/Makefile examples.[5, 6]
- **Ubuntu OnAir — [Do You Want To Build a Kernel?](https://www.youtube.com/watch?v=vVg5N5Crgxo):** beginner questions and a later C/GRUB core, but also a substantial custom-bootsector detour and live confusion between syscall arguments and C-call arguments.[7] The repository's `episode_1` tag explicitly identifies this video.[8] Its startup pushes arguments and calls C without initializing ESP, and its linker script reserves no stack; Multiboot leaves ESP undefined and requires the OS to create its own stack.[9, 10, 11] I would not use it as your first recipe [INFERENCE]. **This is a source-level finding, not an observed test failure.**
- **Ricky Valentine Oswald's Bare Bones recordings:** the reader returned no captions and little descriptive content; that is insufficient evidence to rate their teaching, not evidence that the videos are bad.[12, 13]
- **CodePulse:** targets x86-64/Multiboot2.[14] **Viktor Engelmann:** uses C++ and explicitly assumes some assembly.[15] Neither is an exact match for this request.
- I would also exclude general CPU animations, Linux administration/module lessons, and long homemade-loader series from this shortlist: they do not address the requested lesson directly enough.

## Evidence notes

The cited video metadata, accessible captions, creator transcript, and cited source files were independently read. The research notes preserve the wider candidate and access limits.[16, 17, 18] No audiovisual-quality claim or reproduced build result is asserted. The GNU specification host timed out during rechecking; the stack requirement above was read in the university-hosted HTML copy listed in Sources.[11]

## Removed Unsupported Claims

Removed the claims that these videos address specific questions or difficulties from the learner's earlier lab. The supplied evidence establishes topic coverage, not that personal history.

## Sources

1. sphaerophoria — [Learning (and inferring) about OS bootstrapping](https://www.youtube.com/watch?v=gBykJMqDqH0). Primary metadata, creator chapters, and auto captions; final boot-route uncertainty retained.
2. nanobyte — [Building an OS - 1 - Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc). Primary metadata, description, and auto captions; caption errors noted above.
3. nanobyte — [Creator's almost-verbatim transcript](https://nanobyte.dev/transcripts/building-an-os-1-hello-world/). Explicitly edited for clarity; includes the vocabulary examples, diagrams, and 16-bit BIOS/floppy workflow.
4. Pritam Zope — [Create Your Own Operating System (OS) - Kernel](https://www.youtube.com/watch?v=4hJDOvwbTZs). Primary metadata and description; no captions returned.
5. Pritam Zope's video-linked companion — [Create Your Own Kernel](https://www.c-sharpcorner.com/article/create-your-own-kernel/). Written C/GNU-AS/GRUB/QEMU pipeline, not evidence of spoken teaching.
6. Pritam Zope — [OS repository README](https://github.com/pritamzope/OS). Explicitly distinguishes newer NASM/Makefile examples from older GNU-AS/scripts.
7. Ubuntu OnAir — [Do You Want To Build a Kernel? | YannickOnAir](https://www.youtube.com/watch?v=vVg5N5Crgxo). Primary metadata and auto captions; relevant late C-call correction passages also inspected.
8. Andrea Righi — [PastaOS episode tags](https://github.com/arighi/pastaos/tags). The `episode_1` commit message links this exact Ubuntu OnAir video.
9. Andrea Righi — [PastaOS `episode_1/boot.S`](https://raw.githubusercontent.com/arighi/pastaos/episode_1/boot.S). Complete startup source: argument pushes and C call, no ESP initialization.
10. Andrea Righi — [PastaOS `episode_1/linker.ld`](https://raw.githubusercontent.com/arighi/pastaos/episode_1/linker.ld). Complete episode linker script; no stack reservation.
11. GNU — [Multiboot Specification 0.6.96, §3.2 Machine state](https://www.sra.uni-hannover.de/Lehre/SS22/V_BSB/doc/multiboot.html). HTML copy hosted by Leibniz University Hannover; ESP undefined and OS stack requirement read directly. Used because the GNU-hosted URLs timed out.
12. Ricky Valentine Oswald — [OSDev Bare Bones](https://www.youtube.com/watch?v=8LaZtuZOsGE). Metadata and brief description; no captions returned.
13. Ricky Valentine Oswald — [OSDev Bare Bones & Kitten Space Agency](https://www.youtube.com/watch?v=K4rxKD6L9L4). Metadata; no description or captions returned.
14. CodePulse — [Write Your Own 64-bit Operating System Kernel #1 - Boot code and multiboot header](https://www.youtube.com/watch?v=FkrpUaGThTQ). Creator description and captions explicitly identify x86-64 and Multiboot2.
15. Viktor Engelmann / Write your own Operating System — [Write your own Operating System in 1 hour](https://www.youtube.com/watch?v=1rnA6wpF0o4). Description and captions identify C++ and assumed assembly knowledge.
16. [T1 exact-topic research and access notes](kernel-learning-videos-research-T1.md). Local evidence artifact; bounded candidate coverage and limitations.
17. [T2 teaching-quality research and access notes](kernel-learning-videos-research-T2.md). Local evidence artifact; transcript versus audiovisual assessment boundaries.
18. [Lead direct research and nanobyte bridge evidence](kernel-learning-videos-research-direct.md). Local evidence artifact; comparison baselines and limits of the optional bridge.
