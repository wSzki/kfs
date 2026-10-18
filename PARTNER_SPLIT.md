# Partner implementation split

**Principle: split implementation ownership, not understanding.** Each partner leads some modules, then teaches the other. Both must eventually explain every module and make small changes deliberately.

This is a plan to revisit, not a record of completed implementation or verification. A and B are roles to assign together. [CHECKLIST.md](CHECKLIST.md) remains the source of truth for learning order, demonstrated understanding, runtime evidence, and blockers.

## Work stages

| Stage | Partner A leads | Partner B leads | Shared work and completion evidence |
|---|---|---|---|
| 1. Boot foundation | Work together | Work together | Understand the boot assembly, initial stack, linker script, and minimal Makefile. Boot in QEMU and observe one character on screen. |
| 2. Main modules | VGA screen output and `printk` formatting | GDT construction and assembly that installs it | Agree on shared declarations and initialization order. Integrate working output before relying on it for GDT diagnostics. |
| 3. Complete mandatory work | Human-readable stack dump, using the working output functions | Reproducible build and final bootable image creation | Integrate the modules, boot the kernel and final image, and verify mandatory requirements against the subject and checklist. |
| 4. Cross-teach and defend | Explain and trace B's modules | Explain and trace A's modules | Each partner predicts and performs a small safe change in the other's module, then practices a closed-notes defense. |

### Why this division

- Shared boot and stack foundations make both partners able to reason about later modules.
- Output and GDT implementation can proceed largely independently once that foundation works.
- The stack dump depends on output, so it follows `printk` rather than competing with it.
- Image creation gives B a separate responsibility while A completes the dump.
- Cross-teaching prevents implementation ownership from becoming a permanent knowledge gap.

## Agree on interfaces before working separately

- Settle shared function declarations in `include/kernel.h`.
- Agree on stack-boundary symbols and the required segment selectors.
- Choose one integration owner for `src/kmain.c` and shared initialization changes. Do not edit shared files independently at the same time.
- Integrate at working milestones rather than saving integration until the end.
- Run hardware exercises in QEMU. Predict the result before running, observe actual output, and restore only temporary exercise changes.

## Learning workflow

Use the existing implementation as a reference: read a small piece, understand it, then reproduce or modify it deliberately. Do not rewrite everything merely to divide the typing.

Before splitting into specialist tracks, learn the common foundation together in the order recorded in `CHECKLIST.md`. This ownership plan does not replace that learning sequence.

For each module, the non-author should be able to:

1. Explain what it does and why it is needed.
2. Trace one concrete example through the code.
3. Predict, perform, and explain a small safe change.

Reading the author's comments alone is not enough. Record demonstrated explanations and exercises in `CHECKLIST.md`; keep source predictions separate from observed QEMU results.

## Mandatory work before bonus

Verify the i386 target, required GDT location and descriptors, usable stack, readable stack dump, and submission requirements, including the source, Makefile, bootable image, and 10 MB limit.

Only after the mandatory part is verified should the pair work on the bonus shell (`printstack`, `reboot`, `halt`). Both partners must understand any bonus work too.

## When we return to this plan

Assign A/B and the integration owner, review the current learning checkpoint and build blockers in `CHECKLIST.md`, then choose the next stage from actual evidence rather than assuming the reference implementation already works.
