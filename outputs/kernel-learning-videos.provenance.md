# Kernel-learning videos — provenance

## Status

- **Date:** 2026-10-18.
- **Verification:** PASS WITH NOTES. Independent citation verification and independent fit review completed; review identified no FATAL error and required bounded revisions. The lead applied all review fixes and checked their replacement wording on disk. Final publication matches the revised candidate byte-for-byte; all local artifact targets listed here exist. No independent second review is claimed. Audiovisual suitability remains unassessed.
- **Outcome:** no strong standalone zero-assembly exact-match course established. Final output offers a qualified core-topic discussion and an optional, explicitly non-matching assembly-vocabulary detour. This is not proof that a better video does not exist.
- **Plan:** [approved research plan](.plans/kernel-learning-videos.md).
- **Final report:** [Videos for your first kernel](kernel-learning-videos.md).

## Rounds and division of work

1. Exploratory conversation searches preceded the explicit deep-search request. Two exploratory agents were paused when the formal approval gate became applicable; their preliminary notes were not treated as final research evidence.
2. User approved the written focused-comparison plan with “yes.” Two feynman researchers received separate on-disk briefs and wrote exact-topic and teaching-quality evidence.
3. Lead synthesized the draft and separately investigated the nanobyte vocabulary bridge. No exercise code was written or executed during this research.
4. A separately dispatched feynman verifier checked sources and wrote the cited draft.
5. A separately dispatched feynman reviewer challenged fit, wording, evidence and publication paths.
6. Lead rewrote the final candidate to apply the three MAJOR and two MINOR review fixes. No new empirical or audiovisual claim was added.

## Research and review files

- [T1 brief](.plans/kernel-learning-videos-T1.md) and [T2 brief](.plans/kernel-learning-videos-T2.md).
- [T1 exact-topic research](.drafts/kernel-learning-videos-research-T1.md): 27 numbered sources, including creator code and rejected leads.
- [T2 teaching-quality research](.drafts/kernel-learning-videos-research-T2.md): 24 numbered sources, including transcripts, episode-specific source checks and search log. These overlap with T1; the numbers are not unique-source counts.
- [Lead direct research](.drafts/kernel-learning-videos-research-direct.md): prior baselines, exact-topic discoveries, nanobyte evidence and access limits.
- [Lead draft](.drafts/kernel-learning-videos-draft.md).
- [Independently cited draft](.drafts/kernel-learning-videos-cited.md).
- [Independent fit review](.drafts/kernel-learning-videos-verification.md).
- [Revised final candidate](.drafts/kernel-learning-videos-revised.md).

## Sources accepted in the short report

- [sphaerophoria, Learning (and inferring) about OS bootstrapping](https://www.youtube.com/watch?v=gBykJMqDqH0): primary metadata, creator chapters and auto captions support C/Bare Bones/Multiboot1/assembly/linker/QEMU coverage. Runtime 52:04; published 2023-09-05. Its final GRUB-versus-direct-load route remains uncertain; its admitted guesses limit teaching strength.
- [nanobyte, Building an OS - 1 - Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc): primary metadata, description and auto captions support vocabulary coverage and the 16-bit BIOS-only scope. Runtime 23:18; published 2019-01-26. Not a C/GRUB build recipe.
- [nanobyte's creator transcript](https://nanobyte.dev/transcripts/building-an-os-1-hello-world/): explicitly almost verbatim with small clarity changes; provides examples and diagrams. Agreement of code examples with the edited text does not prove correctness of the original speech. Auto-caption/edited-transcript discrepancies cannot be attributed to ASR, original narration or later editing without audio inspection.

## Important candidates not recommended as standalone courses

- [Pritam Zope: Create Your Own Operating System (OS) - Kernel](https://www.youtube.com/watch?v=4hJDOvwbTZs), 59:54, 2018-01-16. Topic match is supported by the creator description and original [boot.s](https://raw.githubusercontent.com/pritamzope/OS/master/Kernel/Simple/src/kernel_1/boot.s), [build script](https://raw.githubusercontent.com/pritamzope/OS/master/Kernel/Simple/src/kernel_1/run.sh), [linker script](https://raw.githubusercontent.com/pritamzope/OS/master/Kernel/Simple/src/kernel_1/linker.ld), [C source](https://raw.githubusercontent.com/pritamzope/OS/master/Kernel/Simple/src/kernel_1/kernel.c) and [GRUB config](https://raw.githubusercontent.com/pritamzope/OS/master/Kernel/Simple/src/kernel_1/grub.cfg). Captions unavailable, so spoken teaching/pacing unassessed—not judged poor. Original GNU-AS/scripts differ from the repository's newer NASM/Makefile samples; old host gcc/as commands are not portable macOS instructions. The video-linked [companion article](https://www.c-sharpcorner.com/article/create-your-own-kernel/) helps technical mapping, not speech assessment.
- [Ubuntu OnAir: Do You Want To Build a Kernel?](https://www.youtube.com/watch?v=vVg5N5Crgxo), 1:37:56. Some useful novice/expert conversation, but a bootsector detour and live C-call argument confusion. The [episode tags](https://github.com/arighi/pastaos/tags) associate episode_1 with this video; its [startup](https://raw.githubusercontent.com/arighi/pastaos/episode_1/boot.S) pushes arguments/calls C without setting ESP, and its [linker script](https://raw.githubusercontent.com/arighi/pastaos/episode_1/linker.ld) reserves no stack. The [Multiboot specification HTML copy](https://www.sra.uni-hannover.de/Lehre/SS22/V_BSB/doc/multiboot.html), §3.2, leaves ESP undefined and requires the OS to create a stack. This is a source-level rejection, not an observed failure. A later source branch is not substituted for episode one's code.
- [Ricky Valentine Oswald: OSDev Bare Bones](https://www.youtube.com/watch?v=8LaZtuZOsGE) and [Bare Bones & Kitten Space Agency](https://www.youtube.com/watch?v=K4rxKD6L9L4): insufficient description/caption evidence to assess teaching. Not evidence the videos are bad.
- [Cplusplus: How To Create A Simple Kernel In C and NASM](https://www.youtube.com/watch?v=ZJPvKSrfMfs): verified direct video, but no accessible transcript, established GRUB pipeline or clear beginner starting point; described as the final episode of a series.
- [CodePulse's first kernel episode](https://www.youtube.com/watch?v=FkrpUaGThTQ): x86-64 destination/Multiboot2, not the requested kernel path.
- [Viktor Engelmann's one-hour OS](https://www.youtube.com/watch?v=1rnA6wpF0o4): C++/Linux and assumed assembly; previously suggested, not rediscovered as an exact match.
- FlingOS, Queso Fuego, university recording routes, custom-loader recordings, general CPU animations and Linux administration/module videos were examined or screened as documented in T1/T2/direct notes. Missing access or differing prerequisites were not hidden.

## Review fixes applied

- **W1 / MAJOR:** removed “closest” comparative ranking and unsupported first-course strength. The stream is now an optional topic discussion; its C prerequisites are explicit, and the short sample is not sold as a refresher.
- **W2 / MAJOR:** nanobyte is an optional 23-minute BIOS detour, not a prerequisite or replacement recipe. Listed transferable concepts and non-transferable boot details. Added NASM versus GNU-AS/AT&T notation warning; the longer bootstrap discussion is optional/guided, not an automatic next lesson.
- **W3 / MAJOR:** removed draft-relative evidence links from the short report. Provenance uses final-location-correct `.drafts/` and `.plans/` paths; evidence files were not moved or duplicated.
- **W4 / MINOR:** calls the discrepancies observed disagreements, not proven caption errors; states explicitly that no frames or audio were inspected.
- **W5 / MINOR:** removed discarded-claim/editorial bookkeeping and detailed rejection analysis from the short report; retained them here and in the research notes.

## Open questions and evidence limits

- Whether either video actually feels clear, visually readable or comfortably paced to this learner remains unassessed. No video frames/audio were inspected; spoken-topic coverage is not audiovisual-quality evidence.
- Whether an undiscovered polished, zero-prerequisite, exact C/i386/Multiboot1 course exists remains open. Failed discovery does not establish nonexistence.
- Pritam/Oswald spoken explanations remain unverified because captions were unavailable.
- The final successful sphaerophoria boot's exact GRUB/direct-load route remains unresolved; no on-screen commands were frame-checked.
- No verified short nanobyte segment exists in this evidence; do not turn the full 23:18 episode into a claimed five-minute prerequisite.
- No candidate is a verified complete macOS ARM64 setup recipe. Retain the actual project's toolchain/constraints rather than copying host commands from old Linux videos.

## Access and execution notes

- Readers supplied video descriptions, metadata and captions where available. Caption text is imperfect and sometimes one long truncated line; limitations are documented in the evidence files.
- GNU-hosted Multiboot specification URLs timed out during independent verification; the directly read university-hosted HTML copy supplied the retained stack requirement. This is a document copy, not a newly authored standard.
- OSDev pages sometimes returned HTTP 403; search services sometimes ignored exclusions or returned generic Linux/multiboot-USB material. Those summaries were discovery only, not accepted content evidence.
- A browser discovery attempt failed with a detached frame; the subsequent tab list was empty. No tab was left open.
- No PDF parsed, software installed, candidate code compiled, QEMU boot reproduced, or learner exercise file changed during this research. Documentation artifacts are not learning or assignment-verification checkpoints.
