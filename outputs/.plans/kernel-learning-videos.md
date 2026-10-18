# Deep research plan: beginner kernel-learning videos

## User goal

Find genuinely useful videos for a visual learner building a very small kernel for an existing bootloader. The learner knows almost no assembly, is rusty at C, and gets overwhelmed by dense explanations. They want active teaching and to implement exercises themselves, not complete solutions supplied by the assistant.

## Exact target

A minimal 32-bit i386 C kernel, loaded via GRUB/Multiboot1 and run in QEMU. The desired explanation connects kernel.c, the small assembly entry/stack setup, object files, linker script, final kernel ELF, and the first screen output. macOS ARM64 is the host; Linux-based videos are acceptable if setup differences are identified and do not dominate the lesson.

## Key questions

1. Which videos actually follow this path, rather than teaching general CPU electronics, Linux modules, a BIOS-only boot sector, or a different kernel architecture?
2. Which explicitly explain required C and assembly concepts as they appear, instead of assuming them or presenting speedcoding?
3. Is there one strong standalone introduction, or a short sequence of episodes that is clearer?
4. What prerequisites, outdated practices, errors, or host-specific commands could confuse or endanger this learner?

## Evidence required for every finalist

- Exact direct video URL, title, channel, runtime, and publication date when available.
- Creator description/transcript and, where useful, the linked source repository to establish language, architecture, boot protocol, and build/run path.
- Transcript or chapter evidence of teaching style, explained prerequisites, and order of concepts. Text-only inspection can establish spoken explanations, not prove animation quality; label any visual-quality limitation.
- Explicit distinction between exact matches and partial matches.
- Useful starting episode or segment; timestamps only if actually verified.
- Reasons to recommend or reject it for this learner, not merely popularity.

## Scope and exclusions

- Prioritize video. Do not substitute written articles for the requested deliverable.
- Do not repackage the previously offered general CPU introductions as kernel tutorials.
- Do not repeat Viktor Engelmann's C++ walkthrough or CodePulse's x86-64/Multiboot2 series as newly discovered exact matches. They may serve only as comparison baselines.
- A short explanatory bridge video is acceptable only if no single suitable walkthrough exists; clearly identify the gap.
- Do not write exercise code, install tools, change the reference kernel, or resume the lesson during this research.
- Save research artifacts under outputs/; do not update CHECKLIST.md on every search.

## Scale decision

This is a focused comparison, not a general OS-development survey: use two researchers after approval.

- T1: discover and verify exact-topic C / i386 / GRUB-Multiboot kernel walkthroughs.
- T2: independently assess beginner teaching quality and discover overlooked candidates, checking that prerequisites are explained rather than assumed.

The lead owns the ranking and synthesis. A separate verifier checks the cited shortlist, followed by a separate reviewer challenging fit and unsupported claims. Do not add researchers simply to increase volume.

## Deliverable

A short ranked recommendation of at most three videos or tightly bounded video sequences, including one clear first choice if the evidence supports it. For each: direct link, why it fits, prerequisites/mismatches, and where to start. If no strong beginner exact match exists, say so plainly rather than padding the list. Include a compact rejected-candidate appendix in the research file, not in the main chat.

Final files:
- outputs/kernel-learning-videos.md
- outputs/kernel-learning-videos.provenance.md

## Task ledger

| Task | Status | Evidence/output |
|---|---|---|
| Plan and approval | Approved by user on 2026-10-18 | This file |
| T1 exact-topic discovery | Completed; 27-source evidence artifact | outputs/.drafts/kernel-learning-videos-research-T1.md |
| T2 teaching-quality discovery | Completed; 24-source evidence artifact | outputs/.drafts/kernel-learning-videos-research-T2.md |
| Lead synthesis | Revised after review; qualified supplements, not a claimed ideal course | outputs/.drafts/kernel-learning-videos-revised.md |
| Independent source verification | Completed; cited file exists and was read | outputs/.drafts/kernel-learning-videos-cited.md |
| Independent fit review | Completed; no FATAL, 3 MAJOR and 2 MINOR addressed by lead | outputs/.drafts/kernel-learning-videos-verification.md |
| Delivery and provenance | Published and artifact targets verified; PASS WITH NOTES | outputs/kernel-learning-videos.md and outputs/kernel-learning-videos.provenance.md |

## Verification log

- Researchers wrote both evidence files, which the lead read. The lead also verified video metadata/transcripts and recorded discovery limits in `outputs/.drafts/kernel-learning-videos-research-direct.md`. No visual/audio-quality claim is supported.
- Before delivery: confirm all claimed URLs, topics, runtimes/timestamps, and caveats; distinguish verified content from inference; confirm final and provenance files exist.
- Verifier resolved all retained external source links and preserved uncertainty about unavailable captions, audiovisual quality, and the final GRUB boot route. The GNU specification host timed out; a directly read university-hosted HTML copy supports the stack requirement.
- Independent fit review completed. All five findings were addressed in a full revised candidate; targeted grep verified removal of unsupported/process wording and presence of C prerequisites, optionality, 16-bit boundary, syntax warning, and explicit no-frames/audio limitation.
- Final publication compared byte-for-byte with the revised candidate using `cmp`. All plan/brief/evidence/draft/cited/review files referenced by provenance were confirmed to exist. SHA-256 of the published shortlist: `11f79eaff80ff7e004b5c3e09cda642c275d22aa10788f4a4057d08cbf5083aa`.
- Final verification: PASS WITH NOTES. No fabricated visual/audio assessment, no claimed kernel build/boot reproduction, and no unqualified beginner-course recommendation. Open teaching-fit and source-access questions are documented in provenance.

## Decision log

- 2026-10-18: User explicitly requested deep search after finding earlier recommendations too generic or mismatched.
- Exploratory searches and two exploratory delegations began before that request; those delegations are being paused. Their notes, if returned, are preliminary leads only, not approved-plan findings.
- Approval is required before starting the formal research and verification workflow.
- 2026-10-18: User replied “yes”; formal research approved. Two focused briefs written before researcher dispatch.
- 2026-10-18: No strong zero-assembly exact-match standalone course verified. Draft keeps sphaerophoria as a conditional topic match and nanobyte only as a clearly bounded 16-bit assembly-vocabulary bridge; Pritam remains unrated for teaching due to absent captions. No filler third recommendation.
- 2026-10-18: Review narrowed the stream from an unsupported “closest” ranking to a topic-discussion supplement; the optional BIOS vocabulary detour is not a prerequisite. Detailed methods/rejections moved to provenance for a shorter user-facing report. No further review round was required by the protocol because no FATAL issue was found.
