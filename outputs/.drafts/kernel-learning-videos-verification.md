# Kernel-learning videos — independent verification and fit review

Reviewed candidate: `outputs/.drafts/kernel-learning-videos-cited.md` against the approved plan, all three supplied research artifacts, repository learning/toolchain guidance, and the primary sources listed below. This is a read-only evidence and learner-fit review, not a replacement shortlist. Only this report was written. No video frames or audio were inspected; no builds, tests, formatters, installations, or exercise-code changes were performed.

## Summary

The candidate correctly reports that the search **did not establish a strong standalone, zero-assembly course for the exact C/i386/Multiboot1/QEMU task**. Its two selections are different kinds of compromises: sphaerophoria provides inspectable discussion of the relevant Bare Bones pipeline, but is an exploratory learning stream; nanobyte explicitly introduces assembly vocabulary, but does so while building a 16-bit BIOS bootsector. Neither is established as a polished, exact-match beginner course, and the draft generally says so.

The underlying source work is substantially more defensible than an unqualified “best tutorials” list. The remaining problems are bounded: an insufficiently justified comparative heading, an under-specified transfer/viewing path for this particular novice, final-location citation paths, and avoidable audit language. **No FATAL source error was identified.** The lack of audiovisual assessment, inaccessible captions for rejected candidates, and uncertainty about the stream's final GRUB boot are disclosed limitations, not discoveries of false content or reasons to invent a stronger recommendation.

## Strengths

- **[S1] The main negative conclusion stays within the search evidence.** “That is a limit of the candidates and evidence assessed, not a claim that no such video exists” accurately distinguishes an unsuccessful bounded search from proof of nonexistence. The T1 and T2 notes independently decline a strong standalone beginner recommendation.
- **[S2] Metadata and timestamp provenance are sound for the two selections.** Direct reads confirm sphaerophoria's 52:04 runtime and 2023-09-05 publication, and nanobyte's 23:18 runtime and 2019-01-26 publication. Sphaerophoria's description supplies 05:12, 25:00, and 28:13 chapter starts. The draft correctly calls them creator chapters, not frame-verified boundaries; it invents no nanobyte segment timestamp.
- **[S3] Important mismatches are not concealed.** The draft explicitly identifies nanobyte as assembly-only/16-bit/BIOS rather than C/GRUB, prohibits transplanting real-mode boot instructions, and retains the stream's unresolved final GRUB route. The stream transcript actually says it can “get away without setting up grub”; the caveat is warranted, not excessive skepticism.
- **[S4] The Ubuntu rejection has a concrete source basis.** The `episode_1` tag explicitly links the named video. Its startup pushes arguments and calls `main` without establishing ESP; the linker script defines no stack. The inspected Multiboot specification copy leaves ESP undefined and requires the OS to create its own stack. The draft appropriately labels this a source-level finding, not an observed crash or reproduced build failure.
- **[S5] Missing evidence is not converted into negative teaching judgments.** Direct reads returned no captions for Pritam or the two Oswald recordings. Keeping those teaching assessments unrated, rather than calling them bad courses, is appropriate. The CodePulse and Engelmann exclusions are also supported: the former explicitly targets x86-64/Multiboot2, and the latter says C++ and assumes some assembly.

## Weaknesses

### [W1] **MAJOR — comparative recommendation strength exceeds what was established**

**Location:** “1. Closest to the exercise: sphaerophoria,” together with “Where I would start for you [INFERENCE].”

The extensive caveats prevent this from being a fabricated claim of a polished course. Nevertheless, “closest” is an unqualified comparison whose criterion is unclear. Pritam has a source-backed full GRUB pipeline, whereas the selected stream's final boot route is uncertain. The evidence supports choosing the stream because its **spoken topic coverage is inspectable**, not because it is demonstrably the closest technical implementation or best beginner teaching. T2 specifically recommends it only as tutor-guided excerpts.

The proposed first C chapter also does not refresh rusty C. The transcript treats its VGA routines as “nothing ... crazy,” passes over string/pointer work, and moves into linker speculation. A novice's ability to tolerate that excerpt is not evidence that the speaker supplies the missing prerequisites. **[INFERENCE]** The numbered first-choice presentation can therefore feel more reassuring than the evidence warrants, even though the paragraph-level caveats are honest.

**Precise fix:** rename this to an optional, transcript-inspectable **core-topic discussion**, not an overall closest/best course. Keep the explicit no-strong-first-course conclusion. Retain the small 25:00–28:13 sample as a trial if desired, but say directly that it assumes C pointers/arrays/strings and is not a C refresher. Make clear that confusion is a reason to stop, not to consume the rest of the stream. No new candidate or audiovisual inspection is required to make this narrower claim.

### [W2] **MAJOR — the bridge and next-step path need a transfer boundary, not just a mismatch disclaimer**

**Locations:** “Optional vocabulary bridge [INFERENCE]”; “If that makes sense, the 05:12 bootstrapper/Multiboot chapter is the relevant next topic”; and “The creator transcript gives the intended NASM form as destination on the left, source on the right.”

The BIOS mismatch is already prominently disclosed; including this video is **not itself a source error**. The unresolved issue is instructional coherence. With no verified internal segment, nanobyte is a 23:18 whole-episode detour containing boot signatures, real-mode segmentation, and BIOS services as well as the useful vocabulary. Calling it a bridge does not establish that this whole detour is efficient for an overwhelmed learner. The draft does not falsely call it a five-minute segment, but should explicitly acknowledge what is actually being offered.

There is also a notation transition: the repository uses NASM, nanobyte's corrected operand-order explanation is specifically NASM, and the stream discusses GNU assembly/Bare Bones notation. The draft never explains that assembly notation is not uniform across those sources. **[INFERENCE]** Someone who knows almost no assembly can mistake a change of notation or operand convention for a conceptual contradiction. Saying “do not transplant” real-mode code does not explain that separate problem.

Finally, the next creator boundary after 05:12 is 25:00: almost twenty minutes, including the stream's unresolved guesses. It is not another three-minute beginner lesson.

**Precise fix:** keep nanobyte only as an explicitly optional **23-minute vocabulary detour**, not a prerequisite, exact-match lesson, or recommendation to continue its series. Name the transferable vocabulary and keep the existing real-mode exclusions. Add one short warning that NASM and GNU-AS/AT&T notation differ, so the NASM operand-order rule is not universal. Present 05:12–25:00, if retained, as a separate optional guided discussion rather than an automatic next step. Do not invent a shorter timestamp, add a new course, or substitute a written tutorial.

### [W3] **MAJOR — evidence links will break at the planned final location if copied unchanged**

**Location:** Sources 16–18, for example `[T1 exact-topic research and access notes](kernel-learning-videos-research-T1.md)`.

These links are valid relative to the current `.drafts` candidate. They are **not portable unchanged** to `outputs/kernel-learning-videos.md`: there they resolve to `outputs/kernel-learning-videos-research-T1.md` and equivalent T2/direct paths. The inspected directory inventory places all three evidence files under `outputs/.drafts/`, not directly under `outputs/`.

This is a final-delivery blocker **if the candidate is promoted verbatim**, not a claim that the current draft links are already broken or that pending final files should exist at this review stage.

**Precise fix:** when creating the final artifact, use `.drafts/kernel-learning-videos-research-T1.md`, `.drafts/kernel-learning-videos-research-T2.md`, and `.drafts/kernel-learning-videos-research-direct.md` if the notes remain where they are. Alternatively, put these audit references in `outputs/kernel-learning-videos.provenance.md` and link that companion from the final shortlist. Preserve relative paths appropriate to each destination; do not move or duplicate evidence merely to preserve the current links.

### [W4] **MINOR — distinguish caption/transcript disagreement from a diagnosed caption error**

**Locations:** Source 2's “caption errors noted above” and the evidence-limit sentence “not a full audiovisual viewing.”

The main text accurately says the auto captions and edited creator transcript disagree about `mov` direction and stack ordering. However, without listening to the video, the audit cannot decide whether the discrepancy is an ASR error, an error in the original speech, or a later editorial correction. “Caption errors” is stronger than that evidence. Likewise, “not a full audiovisual viewing” is less precise than the actual method: no frames or audio were inspected.

The edited transcript is not an error-free substitute for the recording: its BIOS-interrupt prose says AH=`0Ah` in one sentence while its adjacent table and code use `0Eh`. This does **not** invalidate the inspected vocabulary examples, and the shortlist does not rely on that BIOS value. It reinforces the need to avoid elevating the edited text into proof of correct original narration.

**Precise fix:** change “caption errors” to “auto-caption/edited-transcript discrepancies; original speech not checked.” State plainly that no frames/audio were inspected. Keep the supported NASM rule, but do not imply the original speech was verified as correct. Do not add a long BIOS errata discussion to the learner-facing shortlist.

### [W5] **MINOR — reader-facing audit residue undermines the requested concision**

**Locations:** “Evidence notes,” “Removed Unsupported Claims,” and the lengthy “Why the other candidates did not make the shortlist” section plus 18-entry bibliography.

“The GNU specification host timed out during rechecking” and “Removed the claims that these videos address specific questions or difficulties from the learner's earlier lab” document the research process, not what this learner should watch. The latter refers to a discarded draft claim the reader need never encounter. The full Ubuntu defect explanation is useful provenance but makes the rejected material compete with the two viewing options.

**[INFERENCE]** For a learner explicitly overwhelmed by dense explanations, the accumulation of audit bookkeeping and repeated qualifications makes the answer harder to use. This is a presentation problem, not evidence that the technical caveats should be hidden.

**Precise fix:** remove the “Removed Unsupported Claims” section from the final shortlist. Put source-access history, detailed rejection/stack analysis, and full research-artifact bibliography in the planned provenance companion. Keep the direct video links, the two candidates' material caveats, one compact statement of audiovisual limits, and a small next action in the reader-facing file. A compact rejected-candidate appendix may remain in the research/provenance material; do not turn it into additional recommendations.

## Questions for Authors

- **[Q1]** What does “closest” mean here: boot-path fidelity, inspectable spoken coverage, or beginner suitability? The evidence establishes the second more strongly than the other two. The bounded answer is to label the stream an inspectable supplement rather than require new research to defend an overall ranking. See W1.
- **[Q2]** Is nanobyte intended to be optional enrichment or a necessary step before the stream? Only the former is defensible on the inspected evidence. The final path should state that choice explicitly and must not suggest that 16-bit BIOS work is required for this GRUB-loaded kernel. See W2.

Neither question requires asking the learner for more information or delaying delivery for a new search; both can be resolved by narrowing the final wording.

## Verdict

**Revise before user-facing delivery; no wholesale research restart.** No FATAL finding. There are three MAJOR issues: recommendation framing, viewing-path transfer boundaries, and final-location link portability. Two MINOR issues concern evidence wording and process noise.

The current draft already meets the crucial honesty requirement: it does **not** establish or explicitly claim a polished, zero-prerequisite exact-match course. Do not replace that honest outcome with invented certainty. After the bounded revisions, this can pass as an **evidence-limited shortlist of optional supplements**, not a validated beginner course sequence.

**Confidence: 4/5** for the source/metadata, wording, and artifact-path findings. Learner-fit judgments remain **[INFERENCE]** because audiovisual quality and the learner's actual response to the excerpts were not assessed. No venue-style novelty judgment applies.

### Limitations that are not blockers

| Disclosed limitation | Review disposition |
|---|---|
| No video/audio inspection | Acceptable if no readability, delivery, animation-quality, or measured-pacing claim is made. Do not relabel transcript coverage as visual suitability. |
| Pritam/Oswald captions unavailable | Valid reason to leave teaching unrated, not proof of poor teaching or an obligation to promote them. |
| Stream's final GRUB path uncertain | Keep it a core-topic discussion, not a certified end-to-end GRUB recipe. No runtime reproduction is needed for the narrower recommendation. |
| No nanobyte internal timestamp established | Do not fabricate one. Offer only an optional full-episode detour or omit it. |
| Ubuntu stack finding not executed | Source plus protocol supports rejecting it as a dependable first recipe; retain the distinction from an observed crash. |
| GNU-hosted specification timed out in source verification | The identified university-hosted copy contains the cited machine-state requirement. Record the access history in provenance; do not claim this reviewer retested the GNU host. |

## Revision Plan

1. **W1:** replace the comparative first-choice heading and narrow the recommendation to an optional inspectable supplement; explicitly name the C prerequisites of the 25:00 trial.
2. **W2:** clarify that nanobyte is an optional 23-minute, different-boot-path vocabulary detour; warn about notation transfer; make the longer bootstrap chapter guided/optional, not an automatic next lesson.
3. **W3:** fix evidence links for the final `outputs/` destination or consolidate them in the provenance companion. This is a path edit, not new research.
4. **W4:** report unresolved caption-versus-edited-text disagreement and zero audiovisual inspection precisely.
5. **W5:** remove discarded-claim bookkeeping and move detailed methods/rejections into provenance, while retaining essential caveats beside each candidate.

These steps require no additional candidate discovery, code changes, installations, builds, or audiovisual-quality claims. The lead owns the revisions and final synthesis.

## Inline Annotations

> “## 1. Closest to the exercise: sphaerophoria”

**[W1] MAJOR:** The criterion for “closest” is not established. Pritam has stronger end-to-end GRUB source mapping; this stream has inspectable captions but acknowledged teaching and final-boot uncertainty. Rename it an optional core-topic discussion. This challenges the comparison, not the verified C/Multiboot1 topic coverage.

> “**Where I would start for you [INFERENCE]:** sample the **25:00 C-kernel chapter** first.”

**[W1; Q1] MAJOR:** The timestamp exists, but the segment is not a beginner C refresher. The transcript treats the pointer/string-heavy sample as straightforward. State the prerequisite and allow stopping without implying the learner should understand it already.

> “If that makes sense, the **05:12 bootstrapper/Multiboot chapter** is the relevant next topic.”

**[W2] MAJOR:** Its next creator chapter starts at 25:00, not a few minutes later. It also contains unresolved guesses. Make this a separately optional, guided 05:12–25:00 discussion, not the default next step.

> “**Optional vocabulary bridge [INFERENCE]:** it explicitly introduces **assembly, instructions versus directives, registers, addresses versus contents, and the stack**.”

**[W2; Q2] MAJOR:** The vocabulary coverage is supported. What is not established is an efficient, short bridge for this learner: the available unit is the entire 23:18 BIOS episode. Name that cost/scope, retain optionality, and do not imply this is a prerequisite kernel-build lesson.

> “The creator transcript gives the intended NASM form as destination on the left, source on the right, and describes first-in-last-out stack access.”

**[W2] MAJOR, path-level:** Correctly scoped to NASM, but the proposed path also includes GNU assembly discussion. Add the missing notation warning so the novice does not generalize this operand rule to every assembly source.

> “2. nanobyte — [Building an OS - 1 - Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc). Primary metadata, description, and auto captions; caption errors noted above.”

**[W4] MINOR:** Disagreement with an explicitly edited transcript is observed; the cause of that disagreement is not. Replace “caption errors” with “caption/edited-transcript discrepancies; original speech not checked.”

> “Coverage was assessed through captions, descriptions, and creator material, **not a full audiovisual viewing**.”

**[W4] MINOR:** Say “No video frames or audio were inspected.” The current wording leaves unnecessary ambiguity about whether partial audiovisual assessment happened.

> “16. [T1 exact-topic research and access notes](kernel-learning-videos-research-T1.md). Local evidence artifact; bounded candidate coverage and limitations.”

**[W3] MAJOR at final publication:** Valid from `.drafts`, wrong from the planned `outputs/kernel-learning-videos.md`. Use `.drafts/kernel-learning-videos-research-T1.md` there; apply the analogous fix to Sources 17 and 18, or move these citations to provenance with appropriate paths.

> “## Removed Unsupported Claims”
>
> “Removed the claims that these videos address specific questions or difficulties from the learner's earlier lab.”

**[W5] MINOR:** This is an editorial change log, not a learning recommendation. Remove it from the reader-facing artifact; record it only in provenance if needed.

> “Its startup pushes arguments and calls C without initializing ESP, and its linker script reserves no stack; Multiboot leaves ESP undefined and requires the OS to create its own stack.”

**[S4] Supported:** The episode-tag association, startup file, linker script, and specification copy support the claim. Preserve the following sentence distinguishing this source-level finding from an observed test failure. No build or runtime claim should be added.

## Sources

### Supplied/local material inspected

- [Cited candidate](kernel-learning-videos-cited.md)
- [Approved plan](../.plans/kernel-learning-videos.md)
- [T1 evidence](kernel-learning-videos-research-T1.md)
- [T2 evidence](kernel-learning-videos-research-T2.md)
- [Direct research evidence](kernel-learning-videos-research-direct.md)
- [Repository guidance](../../AGENTS.md) and [learner dashboard/session log](../../CHECKLIST.md); `outputs/` and `.drafts/` directory inventories for path resolution.

### Additional primary URLs inspected during this review

1. sphaerophoria, metadata, creator chapters, and auto-caption transcript: https://www.youtube.com/watch?v=gBykJMqDqH0
2. nanobyte, metadata, description, and auto-caption transcript: https://www.youtube.com/watch?v=9t-SPC7Tczc
3. nanobyte, explicitly edited almost-verbatim transcript: https://nanobyte.dev/transcripts/building-an-os-1-hello-world/
4. Pritam Zope, metadata/description; no captions returned: https://www.youtube.com/watch?v=4hJDOvwbTZs
5. Pritam's video-linked companion, inspected first-boot assembly/linker/C/build material: https://www.c-sharpcorner.com/article/create-your-own-kernel/
6. Pritam's repository README and old/new syntax/build distinction: https://github.com/pritamzope/OS
7. Ubuntu OnAir, metadata and approximately the first 50 KB of the auto-caption transcript, including the custom-loader prelude, GRUB transition, and initial syscall/C-ABI confusion; not a full transcript or audiovisual review: https://www.youtube.com/watch?v=vVg5N5Crgxo
8. PastaOS episode-one startup: https://raw.githubusercontent.com/arighi/pastaos/episode_1/boot.S
9. PastaOS episode-one linker script: https://raw.githubusercontent.com/arighi/pastaos/episode_1/linker.ld
10. PastaOS tag page linking `episode_1` to the exact Ubuntu video: https://github.com/arighi/pastaos/tags
11. Multiboot Specification 0.6.96, university-hosted copy, §3.2 machine state: https://www.sra.uni-hannover.de/Lehre/SS22/V_BSB/doc/multiboot.html
12. Oswald first recording, metadata and caption-access limit: https://www.youtube.com/watch?v=8LaZtuZOsGE
13. Oswald second recording, metadata and caption-access limit: https://www.youtube.com/watch?v=K4rxKD6L9L4
14. CodePulse, explicit x86-64/Multiboot2 description and transcript: https://www.youtube.com/watch?v=FkrpUaGThTQ
15. Engelmann, C++ and assumed-assembly prerequisite in transcript: https://www.youtube.com/watch?v=1rnA6wpF0o4
