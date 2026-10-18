# Videos for your first kernel

**Result:** I did not verify a strong standalone beginner course for your exact **32-bit C + GRUB/Multiboot1 + QEMU** exercise. These are two optional supplements with different uses—not two new projects you need to follow.

## For the actual kernel pipeline

**[sphaerophoria — Learning (and inferring) about OS bootstrapping](https://www.youtube.com/watch?v=gBykJMqDqH0)** — **52:04**.

It discusses the right topics: a C kernel, assembly entry and stack setup, Multiboot1, object files, linker sections, and QEMU.[1]

**A small trial [INFERENCE]:** watch the creator's **25:00–28:13 C-kernel chapter** rather than the whole stream. It assumes C pointers, arrays and strings; it is **not a C refresher**. If those are unclear, stop there—not understanding this segment does not mean you should push through the rest.

The **05:12–25:00 bootstrap chapter** is a separate, longer discussion that we could unpack together, not an automatic next lesson. Both ranges come from the creator's chapter starts.[1]

**Catch:** this is someone learning aloud, including guesses, unresolved explanations, and setup debugging—not a polished beginner class. Its final successful boot is not clearly established as using the rebuilt GRUB ISO rather than direct Multiboot loading. Treat it as discussion material, not an authoritative recipe.[1]

## Only if assembly vocabulary is the blocker

**[nanobyte — Building an OS, Part 1: Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc)** — **23:18**.

It explicitly introduces registers, instructions versus directives, addresses versus contents, and the stack. That makes it a plausible vocabulary supplement [INFERENCE].[2,3]

**Catch:** this is a **23-minute detour into a 16-bit BIOS bootsector**, not a C/GRUB kernel lesson and **not a prerequisite** for our exercise. No shorter segment was verified. Do not copy its `org 0x7C00`, real-mode addressing, BIOS printing calls, or floppy-image build into your lab. There is no recommendation to follow its whole series.[2,3]

Also, its NASM notation differs from the GNU-AS/AT&T notation discussed in the other recording and shown by your `objdump`: operand order is not universal. Its auto captions and edited creator transcript disagree on some technical wording; the original speech was not checked.[2,3]

## Recommendation boundary

For a directly relevant sample, try the small sphaerophoria chapter above. Skip nanobyte unless you specifically want the extra assembly vocabulary. **Neither has been established as the great, zero-prerequisite course you asked for.**

I checked metadata, captions and creator material; **no video frames or audio were inspected**. Actual screen readability, delivery and pacing remain unassessed. Neither option is a macOS ARM64 setup guide.

## Sources

1. [sphaerophoria: primary video, description, creator chapter markers and auto captions](https://www.youtube.com/watch?v=gBykJMqDqH0).
2. [nanobyte: primary video, description, metadata and auto captions](https://www.youtube.com/watch?v=9t-SPC7Tczc).
3. [nanobyte: creator's almost-verbatim transcript, explicitly edited for clarity](https://nanobyte.dev/transcripts/building-an-os-1-hello-world/).
