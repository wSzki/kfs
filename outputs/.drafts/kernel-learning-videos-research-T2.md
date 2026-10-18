# T2 — Teaching quality and independent video discovery

Research date: 2026-10-18. Scope: beginner video teaching for a small **32-bit i386 C kernel, GRUB/Multiboot1, QEMU**; no builds, installations, or exercise-code changes. This is evidence for the lead, not a final ranking.

## Coverage Status

- **Done — independent discovery:** broad kernel/GRUB/Bare Bones searches, university-course routes, small-channel recordings, and FlingOS resources; three substantive video candidates assessed below. No strong standalone beginner match was verified.
- **Done — teaching assessment:** full auto-caption transcript for sphaerophoria [1]; a large opening/middle transcript excerpt plus targeted late-transcript passages for Ubuntu OnAir [7]; creator description and companion article for Pritam Zope [2][22].
- **Done — technical cross-check:** Ubuntu's episode-specific `episode_1` source, not just its later branch [9–14]; GNU Multiboot1 reference [15][16]; Pritam's creator-linked article [22].
- **Blocked — spoken/visual assessment:** no captions for Pritam Zope or the Oswald recordings [2][4][5]; no video/audio/frame inspection was available. This prevents claiming clear visual design, calm delivery, or sufficient explanations in those videos.
- **Uncertain, deliberately not promoted:** FlingOS's direct episode URLs/captions/runtimes were not recoverable from its primary playlist shell. Its inspected C example uses an ISOLINUX/SYSLINUX loader and Windows build tools, not the target GRUB workflow [17–21].

## Teaching-quality decision for synthesis

**No strong standalone first choice is supported by this slice.** sphaerophoria is the most inspectable exact-topic explanation here, but it is exploratory, assumes considerable background, and contains unresolved guesses; at most, use it as a **tutor-guided excerpt**, not an independently sufficient novice course [1]. Pritam Zope is technically aligned but its spoken teaching cannot be assessed from available evidence [2][22]. Ubuntu OnAir has an engaging novice/expert dialogue and the correct later topic, but a long custom-bootsector prelude, live ABI confusion, and an episode-one stack-initialization omission make it a poor first recipe for this learner [7][9][16]. These are evidence judgments, **not a final ranking**.

## Evidence table

Stable IDs are retained throughout this artifact. Confidence concerns the recorded claim, not global tutorial quality.

|#|Source|URL|Key claim inspected directly|Type|Confidence|
|---|---|---|---|---|---|
|1|sphaerophoria, *Learning (and inferring) about OS bootstrapping*|https://www.youtube.com/watch?v=gBykJMqDqH0|2023-09-05; 52:04; creator description, chapters, and auto-generated transcript available. Walks through Bare Bones, C VGA output, assembly startup, linking, and QEMU; explicitly compares Multiboot1 and 2 and identifies 1.|primary; auto-caption text|high for topic/metadata; medium for exact spoken wording|
|2|Pritam Zope, *Create Your Own Operating System (OS) - Kernel*|https://www.youtube.com/watch?v=4hJDOvwbTZs|2018-01-16; 59:54. Creator description specifies C Hello World, strings/numbers, linked list, GNU assembler, Kali Linux i386, GCC, GRUB rescue ISO, QEMU. No captions available.|primary; creator description|high for description; teaching unknown|
|3|Pritam Zope, OS repository|https://github.com/pritamzope/OS|Creator-linked repository contains C kernels and Linux tool requirements. README warns the newer `NEW KERNEL` examples changed assembly from GNU AS to NASM and scripts to Makefiles.|primary creator code/README|high|
|4|Ricky Valentine Oswald, *OSDev Bare Bones*|https://www.youtube.com/watch?v=8LaZtuZOsGE|2026-09-17; 33:34. Description says only “Following osdev wiki Bare Bones.” No captions available.|primary metadata/description|high for metadata; insufficient technical/teaching evidence|
|5|Ricky Valentine Oswald, *OSDev Bare Bones & Kitten Space Agency*|https://www.youtube.com/watch?v=K4rxKD6L9L4|2026-09-17; 43:57. No description or captions retrieved.|primary metadata only|high for metadata; insufficient content evidence|
|6|Third-party mirror of sphaerophoria stream|https://rutube.ru/video/c5ee78917f54fb98a4b0f1704603da9d/|HTML names original YouTube URL `gBykJMqDqH0` and reproduces description/chapters. Used to recover the primary URL, not to establish teaching quality.|secondary mirror/discovery only|medium|
|7|Ubuntu OnAir, *Do You Want To Build a Kernel? \| YannickOnAir*|https://www.youtube.com/watch?v=vVg5N5Crgxo|2021-12-14; 1:37:56; English auto captions. Starts with a custom BIOS bootsector, then switches to a GRUB-loaded 32-bit C kernel. Includes explanations, co-host questions, live build debugging, and ABI confusion.|primary; auto-caption text|high for metadata/topic; medium for exact spoken wording|
|8|Ubuntu's official discussion, including Andrea Righi's source links|https://discourse.ubuntu.com/t/ubuntuonair-discussion-do-you-want-to-build-a-kernel/25788|Connects the exact video to `arighi/pastaos`, its `ubuntuonair` branch, and episode tags. Prevents treating an unrelated repository as video evidence.|primary creator/platform discussion|high|
|9|PastaOS `episode_1` assembly entry|https://raw.githubusercontent.com/arighi/pastaos/episode_1/boot.S|Multiboot1 magic `0x1BADB002`, `_start`, argument pushes and `call main`; **no assignment to ESP / stack allocation** in this complete short file.|primary episode-specific code|high|
|10|PastaOS `episode_1` Makefile|https://raw.githubusercontent.com/arighi/pastaos/episode_1/Makefile|`-m32`, `ld -m elf_i386`, GRUB ISO creation, and QEMU. The `qemu-system-x86_64` executable name does not imply the kernel code is 64-bit.|primary episode-specific code|high|
|11|PastaOS `episode_1` linker script|https://raw.githubusercontent.com/arighi/pastaos/episode_1/linker.ld|Defines `_start`, `.text` at `0x100000`, `.data`, and `.bss`; does not reserve or define a kernel stack.|primary episode-specific code|high|
|12|PastaOS `episode_1` C kernel|https://raw.githubusercontent.com/arighi/pastaos/episode_1/main.c|Small C VGA text driver and `main`; does not perform stack setup before the assembly call.|primary episode-specific code|high|
|13|PastaOS episode tags API|https://api.github.com/repos/arighi/pastaos/tags|`episode_1` resolves to `2005cc860331a116845549e2bc457ed3cb90d015`; later episode tags also exist.|primary repository metadata|high|
|14|PastaOS later `ubuntuonair` assembly entry|https://raw.githubusercontent.com/arighi/pastaos/ubuntuonair/boot.S|Later branch includes `mov $KERNEL_STACK_BEGIN, %esp`; it is not interchangeable with episode one's source.|primary creator code; later state|high|
|15|GNU Multiboot1 specification — Header magic fields|https://www.gnu.org/software/grub/manual/multiboot/html_node/Header-magic-fields.html|Header magic is `0x1BADB002`; magic + flags + checksum must sum to zero as 32-bit unsigned values.|primary standard|high|
|16|GNU Multiboot1 specification — Machine state|https://www.gnu.org/software/grub/manual/multiboot/html_node/Machine-state.html|Bootloader invokes a 32-bit OS; ESP is undefined and “The OS image must create its own stack as soon as it needs one.”|primary standard|high|
|17|FlingOS Getting Started tutorial resources|https://github.com/FlingOS/Getting-Started-Tutorials|Incremental assembly/C/C# samples; README explicitly says videos do not show every part of the code and neither videos nor resources should be used in isolation.|primary creator resources|high|
|18|FlingOS main repository|https://github.com/FlingOS/FlingOS|Links the official tutorial playlist, `PLKbvCgwMcH7BX6Z8Bk1EuFwDa0WGkMnrz`; the main project is C#, while tutorial resources include C.|primary creator README|high|
|19|FlingOS creator's Getting Started article source|https://raw.githubusercontent.com/FlingOS/FlingOS/master/Documentation/Markdown/Reference%20Articles/Getting-Started.markdown|Dated 2015-08-30; claims ten approximately 20-minute videos taking existing C/C# programmers through basic x86 OS development. This is a creator claim, not independently verified episode timing/quality.|primary; self-reported course intent|high for stated intent only|
|20|FlingOS “Using C” build script|https://raw.githubusercontent.com/FlingOS/Getting-Started-Tutorials/master/5.%20Using%20C/Code/Build.bat|Windows `.bat`, NASM executable, `i686-elf-gcc`, and ISO generation using `isolinux-debug.bin`.|primary creator code|high|
|21|FlingOS “Using C” boot configuration|https://raw.githubusercontent.com/FlingOS/Getting-Started-Tutorials/master/5.%20Using%20C/Code/ISO/syslinux.cfg|Uses `mboot.c32` to load `Kernel.bin`, not a GRUB configuration.|primary creator code|high|
|22|Pritam-video-linked companion, *Create Your Own Kernel*|https://www.c-sharpcorner.com/article/create-your-own-kernel/|First-boot section shows C, Multiboot1 magic, GNU-AS entry/ESP setup, linker script, `multiboot` GRUB config, host `as`/`gcc`, ISO and QEMU. Some prose is technically imprecise.|primary creator-linked companion; not transcript|high for displayed pipeline; speech unknown|
|23|Mythili Vutukuru, IIT Bombay OS lecture index|https://www.cse.iitb.ac.in/~mythili/os/|Course distinguishes general OS concepts from an xv6 deep dive. Not evidence of a small GRUB/Multiboot1 from-scratch C lesson; not substituted for the requested video.|primary course index|high|
|24|dr Jonas Birch, *Project: Coding a GRUB bootloader for booting Linux*|https://www.youtube.com/watch?v=ukIajayMv5U|2025-10-02; 2:10:24; manual transcript. Explicitly writes a custom 16-bit partition-reading/relocating bootloader to load Linux; title's “GRUB” does not make it the requested C-kernel lesson.|primary; manual captions|high|

## Candidate teaching assessments

### A. sphaerophoria — exact-topic, but not a polished novice lesson

Direct video: https://www.youtube.com/watch?v=gBykJMqDqH0 . English auto-caption transcript, 52:04, uploaded 2023-09-05. [1]

**Verified useful content:** the creator’s goal is explicitly to boot a kernel in QEMU “with the ability to understand what's going on.” The opening uses a normal C Hello World plus `strace`/dynamic-library inspection to explain why Linux-dependent application code is not the desired bare-metal output. Later the transcript works through `.multiboot`, stack allocation, `%esp`, `_start`, the C entry point, VGA memory, linker sections, object compilation, a GRUB configuration/ISO, and running the result. [1]

**Teaching benefit [INFERENCE]:** Unlike silent code copying, the spoken questions expose why each layer exists. The explanation of the preallocated stack as avoiding overwriting code, and the distinction between symbols/names and addresses resolved by linking, could work as material for active tutor-guided pausing. [1]

**Important counterevidence:** This is a live learning stream, not a prepared lesson. The speaker repeatedly says “I think,” “I don't remember,” and “I'm just guessing”; initially guesses incorrectly about `.set`, struggles to inspect BSS, is unsure about entry-point interpretation and why the link address is 1 MiB, and spends time resolving Make/PATH/NixOS setup. He explicitly ends by saying “all we did is copy paste.” Do **not** market this as authoritative first-principles teaching for someone with almost no assembly. [1]

**Verified creator chapter starts:** 00:00 intro; 01:40 cross-compiler rationale; 05:12 bootstrapper/Multiboot; 25:00 sample C kernel; 28:13 linker script; 34:20 trying it for real. These are description chapters, not independently frame-checked cut points. [1]

**Host/prerequisites:** a cross-compiler has been built before the recording; the live setup is NixOS/Linux, not macOS ARM64. The opening assumes enough C and Unix tooling to follow `strace`, shared libraries, disassembly, and Make rules; the C VGA routines are largely treated as straightforward. [1]

**Protocol/run caveat:** the transcript explicitly checks Multiboot1 against Multiboot2 rather than using the current Rust codebase as evidence. It does build a GRUB configuration and ISO, but the closing debug sequence also says they can “get away without setting up grub” while testing. Do not claim that every final QEMU invocation shown necessarily boots the regenerated GRUB ISO; this is an exact-topic Bare Bones discussion, not a frame-verified end-to-end GRUB recipe. [1]

**Assessment:** conditional exact-topic supplement only; **not an evidence-backed strong standalone beginner recommendation**. If the lead includes it, the most defensible bounded teaching block is the creator's 05:12 bootstrapper chapter up to the 25:00 C chapter; even this needs an active tutor to stop at guesses and define assembly vocabulary. The 28:13 linker discussion itself contains uncertainty, so it should not be offered as a self-sufficient linker explanation. Visual readability, animations, audio delivery, and screen pace remain unassessed. [1]

### B. Pritam Zope — technically aligned; spoken teaching unverified

Direct video: https://www.youtube.com/watch?v=4hJDOvwbTZs . 59:54; uploaded 2018-01-16. **Spoken language unknown**: the description and linked article are English, but no captions were retrieved and audio was not inspected. [2][22]

**Verified topic:** the creator description promises a C Hello World kernel, strings/numbers, and a singly linked list. The linked companion's first-boot section supplies Multiboot1 magic, GNU-AS startup and ESP assignment, an entry-point/linker script, `multiboot` in `grub.cfg`, GRUB rescue ISO creation, and QEMU. This supports exact-topic classification much more strongly than title alone; the i386 Linux development environment is explicitly named in both video description and companion. [2][22]

**Teaching evidence boundary:** the written companion comments the stack and separates assembly, linker, GRUB config, C VGA output, and build commands. This shows a potentially useful conceptual order **in the article**, not that the video explains those things aloud. Registers from scratch, pointer refreshers, object-file meaning, spoken pacing, and visual clarity are all unverified. Do not rate it “beginner-friendly” merely because the kernel is small or the description says Hello World. [2][22]

**Host/toolchain caveat:** the article's `run.sh` calls plain `as` and `gcc` under the stated i386 Linux host; those are not portable macOS ARM64 instructions. The repository now contains newer NASM/Makefile examples, whereas this video says GNU AS and the linked first-boot article uses scripts. A novice must not mix those versions. [2][3][22]

**Conceptual cautions:** the companion prose says the checksum is the sum of magic and flags, while its actual assembly correctly negates that sum; GNU requires the three fields to sum to zero. Its surrounding description of section size/alignment is also imprecise. **[INFERENCE]** The discrepancy makes the companion a weak independent authority for a beginner even though the displayed boot pipeline matches. [15][22]

**Assessment:** keep as an exact-topic **unrated lead**, not a confidently beginner-fit recommendation. The extra linked-list/number-formatting scope may overwhelm this learner [INFERENCE]; no verified timestamp isolates first boot. [2]

### C. Ubuntu OnAir / Andrea Righi with Yannick — exact core, mixed whole-video path; reject as first recipe

Direct video: https://www.youtube.com/watch?v=vVg5N5Crgxo . *Do You Want To Build a Kernel? | YannickOnAir*, 1:37:56, uploaded 2021-12-14, English auto-caption transcript. Ubuntu's official discussion links this video to Righi's code and episode tags. [7][8][13]

**Exact versus partial:** the entire recording is **not** a clean GRUB-first walkthrough: it starts by writing a custom 512-byte, 16-bit BIOS bootsector. It later explicitly discards that work and lets GRUB handle booting/protected-mode setup, then creates a 32-bit C kernel, linker script, GRUB ISO, and QEMU run. Thus: **exact-topic later core, partial/mixed-path whole video**. Episode-one `-m32` and `elf_i386` establish the guest architecture despite the use of a QEMU executable named `qemu-system-x86_64`. No timestamp for the switch was verified; none is supplied here. [7][9–12]

**Positive teaching evidence:** the co-host asks genuine beginner questions, including when they will switch from assembly to C. Righi defines `.text`, initialized `.data`, and uninitialized `.bss` using small C examples; explains why entry points and custom linker scripts exist; explains why `stdio`/libc cannot simply be used; and illustrates doing nothing with an inefficient busy loop versus halt. **[INFERENCE]** This conversational format is more actively explanatory than a silent speedcode recording. [7]

**Negative teaching evidence:** there are chat digressions, an audio-echo troubleshooting discussion, Makefile/dependency fixes, and live guessing. Righi explicitly says he is going fast through some tooling because he wants to show the “cool things,” not spend time on the “boring part.” Most importantly for a novice, he initially uses the Linux syscall register-argument table as if it specified C calls; later disassembly shows the C function reading arguments from ESP, so he changes to stack pushes while admitting uncertainty about why. The later correction does not turn the earlier explanation into a clear calling-convention lesson. This is transcript evidence, not a claim about the creators' overall expertise. [7]

**Source-level acceptance blocker, not a runtime-test result:** the video-linked `episode_1` tag's complete startup file pushes arguments and calls C without initializing ESP, and the associated linker script reserves no stack. GNU Multiboot1 explicitly leaves ESP undefined and requires the image to create its stack before needing one. **[INFERENCE from source + standard]** Following that episode-one recipe therefore relies on undefined entry-state stack memory and is unsuitable as a dependable first kernel recipe. I did not execute it and do not claim an observed crash. The later `ubuntuonair` branch adds an ESP initialization, so citing the later branch would conceal the episode-one omission. [9][11][13][14][16]

**Host/prerequisites:** GNU/Linux-oriented GCC/linker/GRUB commands and QEMU console mode; not a macOS ARM64 setup walkthrough. The C portion proceeds with integer typedefs, casts, pointers, bit shifts, and a VGA buffer without being a C refresher. [7][10][12]

**Assessment:** not recommended for this user's first standalone viewing or as code to imitate. It may interest a later learner who already understands C calls and startup stacks, but the long off-target prelude and correction burden are especially poor fits for someone easily overwhelmed. This is an inferred learner-fit judgment grounded in the points above. [7][9][16]

### Teaching-coverage comparison

“Explained” below means supported by speech/transcript, not inferred from source code.

|Candidate|Registers / assembly vocabulary|Stack / C entry|Objects / linking|C refresher|Beginner-fit conclusion|
|---|---|---|---|---|---|
|sphaerophoria [1]|Names/register conventions mostly assumed; think-aloud lookups|Discusses preallocated stack and ESP; entry interpretation partly guessed|Examines sections/symbols, then guesses about some linker behavior|VGA routines largely treated as obvious|Only as guided exact-topic excerpts|
|Pritam Zope [2][22]|Spoken explanation unknown|Written startup comments only; speech unknown|Written pipeline available; speech unknown|Unknown|Exact-topic but teaching quality unproven|
|Ubuntu OnAir [7][9–12]|Some definitions, including register high/low parts|C-call argument confusion corrected experimentally; episode-one source lacks initialized stack|Explicit sections/linker discussion, with live tooling work|Not a pointer/bitwise refresher|Reject as first recipe; later exact core does not erase omissions|

## Search and rejection log

Search results are discovery only; claims above use directly read primary pages.

|Query/route|Outcome / rejection|
|---|---|
|`C kernel GRUB multiboot tutorial video beginner 32 bit`|Mostly written Bare Bones results; discovered mirror of sphaerophoria stream. Not treated as a video recommendation until original URL and transcript were fetched.|
|`"kernel" "bare bones" "youtube" grub C`|Mostly written results; no supported new recommendation.|
|`"writing" "C kernel" "video" multiboot`|CodePulse x86-64 lead recurred; already excluded as exact by assignment.|
|`site:youtube.com/watch "kernel" "GRUB" "C" tutorial -64 -offlinemark -Engelmann`|Search engine relaxed the site constraint and returned off-target Linux/GRUB administration pages. No content claims retained.|
|`site:youtube.com/watch "bare bones" "operating system"`|Discovered Oswald recordings [4][5], but captions/content evidence inadequate. Also returned a Windows XP feature-removal video, excluded immediately as unrelated.|
|`"multiboot" "video tutorial" "32" "C"`|Mostly written tutorials and specifications; no new supported video.|
|`grub kernel tutorial YouTube`|Linux boot-management results; rejected because configuring Linux GRUB is not writing a freestanding C kernel.|
|`multiboot kernel lecture video`|Mostly written specifications or broad OS course pages; not promoted without an applicable direct video.|
|Exact title / sphaerophoria + bootstrapping searches|Mirror exposed original URL; original fetched and transcript assessed [1][6]. A separate Bilibili result was a later Rust episode, not this C introduction.|
|`"Daedalus Community" "operating system"`|Search was noisy; no exact-topic source verified from this route. No unsupported named recommendation retained.|
|`"OSDev" "GRUB" "youtube.com/watch" "C"`; `"create" "operating system" "multiboot" "YouTube"`|Mostly known mismatches and written references. Custom-bootloader and 64-bit routes not promoted.|
|`"kernel" "multiboot" "tutorial" "video"` with exclusions; `"osdev" "grub" "YouTube" "beginner"`|Known x86-64, custom-loader, written, and already-rejected live-stream leads dominated; no new beginner-fit finalist.|
|`"kernel" "multiboot" "lecture" "YouTube"`; primary IIT Bombay course page|University route inspected [23]; structured OS/xv6 theory is not automatically the requested tiny GRUB kernel lesson. Not substituted for the requested path.|
|`"FlingOS" "Getting Started" "video"`; `"FlingOS: Creating an OS"`; `"FlingOS" "Using C"`|Reached creator resources/playlist through [17–19]. Playlist primary URL is https://www.youtube.com/playlist?list=PLKbvCgwMcH7BX6Z8Bk1EuFwDa0WGkMnrz . Reader only exposed a shell, no direct episode IDs/captions. “Using C” source uses Windows tools plus ISOLINUX/SYSLINUX [20][21], so not an exact GRUB match. Not promoted.|
|`"kernel" "linker script" "video" "beginner"` → exact workshop title / `kernel YannickOnAir Ubuntu`|Discovered Ubuntu OnAir workshop via a course index, then fetched primary video, official discussion, exact episode tag, and code [7–14]. Rejected as first recipe for reasons above.|
|`"noyau" "C" "GRUB" "vidéo"`|French-language route returned mostly Linux administration/written material; no supported exact tutorial identified.|
|`"minimal kernel" "YouTube" "32"`; `"first kernel" "tutorial" youtube C`; `"multiboot1" "youtube" kernel`|Minimal-kernel and explicit-protocol routes produced written sources, CUDA/kernel-module false positives, or no directly verifiable additional lesson. Not filled with generic CPU videos.|
|Discovered `ukIajayMv5U` from broad OSDev/GRUB search|Primary title is *Project: Coding a GRUB bootloader for booting Linux*. Transcript establishes custom 16-bit loader/partition handling, not writing the requested C kernel. Rejected despite misleading keyword overlap. [24]|
|Pritam creator-linked article routes|C# Corner first-boot content readable [22]; CodeProject connection refused. Article used only for technical mapping, not as a replacement deliverable or proof of spoken teaching.|

## Access limits / methodology

- `read` fetched auto captions for [1] and [7], but reported **no subtitles/captions** for [2], [4], and [5]. Absence is an access limit, not proof the teaching is bad.
- For [7], the transcript occupied one approximately 65.6 KB line; raw reading exposed the first 50 KB, and targeted searches retrieved late ABI/correction passages. I do not claim complete second-by-second review of the entire recording. A `t=` read normalized to the same full-video transcript; it did not supply verified segment timing.
- `https://wiki.osdev.org/Bare_Bones`, `https://wiki.osdev.org/Barebones`, `https://wiki.osdev.org/Tutorials`, the FlingOS HTTP/HTTPS sites, and an OSDev FlingOS announcement returned HTTP 403. The creator's old blog URL returned HTTP 404. FlingOS repositories remained readable. [17–21]
- The Ubuntu workshop's third-party course-listing classroom link returned HTTP 403, and a raw retry of the listing returned HTTP 429; these did not block primary video/forum/code access. No course-listing summary was used as teaching evidence.
- The Pritam-linked CodeProject article returned `ECONNREFUSED`; the other linked companion was readable [22].
- No claimed visual inspection: transcripts cannot prove font readability, diagrams, animations, pace of on-screen edits, or how visually supportive the presentation is. Audio/accent/calmness assessments are also unavailable.
- No timestamps invented. Only [1]'s creator-supplied chapters are reproduced; source quotes from [7] are untimestamped.
- Search occasionally relaxed constraints or returned prose recommendations for the wrong task. Only fetched, content-matched primary evidence was promoted; mirrors were used to recover original URLs.
- No code was executed, compiled, installed, or modified. Source-level defect findings are explicitly distinguished from observed test results.

## Sources

1. sphaerophoria — *Learning (and inferring) about OS bootstrapping* — https://www.youtube.com/watch?v=gBykJMqDqH0
2. Pritam Zope — *Create Your Own Operating System (OS) - Kernel* — https://www.youtube.com/watch?v=4hJDOvwbTZs
3. Pritam Zope — OS repository — https://github.com/pritamzope/OS
4. Ricky Valentine Oswald — *OSDev Bare Bones* — https://www.youtube.com/watch?v=8LaZtuZOsGE
5. Ricky Valentine Oswald — *OSDev Bare Bones & Kitten Space Agency* — https://www.youtube.com/watch?v=K4rxKD6L9L4
6. RUTUBE mirror — *Learning (and inferring) about OS bootstrapping* — https://rutube.ru/video/c5ee78917f54fb98a4b0f1704603da9d/
7. Ubuntu OnAir — *Do You Want To Build a Kernel? | YannickOnAir* — https://www.youtube.com/watch?v=vVg5N5Crgxo
8. Ubuntu Discourse / Andrea Righi — *UbuntuOnAir Discussion: Do You Want to Build a Kernel* — https://discourse.ubuntu.com/t/ubuntuonair-discussion-do-you-want-to-build-a-kernel/25788
9. Andrea Righi — PastaOS `episode_1/boot.S` — https://raw.githubusercontent.com/arighi/pastaos/episode_1/boot.S
10. Andrea Righi — PastaOS `episode_1/Makefile` — https://raw.githubusercontent.com/arighi/pastaos/episode_1/Makefile
11. Andrea Righi — PastaOS `episode_1/linker.ld` — https://raw.githubusercontent.com/arighi/pastaos/episode_1/linker.ld
12. Andrea Righi — PastaOS `episode_1/main.c` — https://raw.githubusercontent.com/arighi/pastaos/episode_1/main.c
13. GitHub — PastaOS episode tags — https://api.github.com/repos/arighi/pastaos/tags
14. Andrea Righi — PastaOS later `ubuntuonair/boot.S` — https://raw.githubusercontent.com/arighi/pastaos/ubuntuonair/boot.S
15. GNU — Multiboot Specification 0.6.96, Header magic fields — https://www.gnu.org/software/grub/manual/multiboot/html_node/Header-magic-fields.html
16. GNU — Multiboot Specification 0.6.96, Machine state — https://www.gnu.org/software/grub/manual/multiboot/html_node/Machine-state.html
17. FlingOS — Getting Started tutorial resources — https://github.com/FlingOS/Getting-Started-Tutorials
18. FlingOS — Main project README and official playlist link — https://github.com/FlingOS/FlingOS
19. FlingOS — Getting Started article source — https://raw.githubusercontent.com/FlingOS/FlingOS/master/Documentation/Markdown/Reference%20Articles/Getting-Started.markdown
20. FlingOS — “Using C” `Build.bat` — https://raw.githubusercontent.com/FlingOS/Getting-Started-Tutorials/master/5.%20Using%20C/Code/Build.bat
21. FlingOS — “Using C” `syslinux.cfg` — https://raw.githubusercontent.com/FlingOS/Getting-Started-Tutorials/master/5.%20Using%20C/Code/ISO/syslinux.cfg
22. Pritam-video-linked companion — *Create Your Own Kernel* — https://www.c-sharpcorner.com/article/create-your-own-kernel/
23. Mythili Vutukuru — IIT Bombay, *Lectures on Operating Systems* — https://www.cse.iitb.ac.in/~mythili/os/
24. dr Jonas Birch — *Project: Coding a GRUB bootloader for booting Linux* — https://www.youtube.com/watch?v=ukIajayMv5U
