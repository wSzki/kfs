# KFS_2 — GDT & Stack (42)

Bare-metal i386 kernel that installs its own Global Descriptor Table at
physical address `0x800`, wires up the kernel stack, prints the stack in a
human-readable hexdump, and (bonus) offers a small debug shell
(`printstack` / `reboot` / `halt`).

**This repo is written as a tutorial**: every file opens with a header
explaining its role in the boot chain, and every hardware trick is commented
bit-by-bit. Read top-to-bottom in the order of §2 below and you should be
able to rebuild the whole thing from scratch yourself.

---

## 1. What actually happens when you boot

```
BIOS (16-bit real mode)
 └─ GRUB (bootloader)
     ├─ finds the multiboot header in our first 8 KiB   → boot.asm
     ├─ loads kernel.elf at ≥ 1 MiB                     → linker.ld
     ├─ switches CPU to 32-bit protected mode (GRUB's own temporary GDT)
     └─ jumps to _start with EAX = magic, EBX = info pointer
         └─ _start: sets ESP to our stack, pushes args, calls kmain
             ├─ printk()         screen + serial output   (src/printk.c)
             ├─ gdt_install()    builds GDT at 0x800      (src/gdt.c)
             │   └─ gdt_flush()  lgdt + segment reload   (src/gdt_flush.asm)
             ├─ stack_dump()     hexdump of used stack    (src/stack_dump.c)
             └─ shell_run()      debug shell, never rets  (src/shell.c)
                                     └─ polling keyboard   (src/keyboard.c)
```

## 2. Suggested reading order (complete solution → drill-down)

1. `include/kernel.h` — the map: boot flow, GDT layout, selector math.
2. `boot.asm` — the multiboot contract and "no stack, no C".
3. `linker.ld` + `Makefile` — where code lands in memory and why each
   compiler flag exists.
4. `src/printk.c` — MMIO (0xB8000), scrolling, printf from scratch,
   UART serial output.
5. `src/gdt.c` + `src/gdt_flush.asm` — the core of the subject: descriptor
   bytes, `lgdt`, far jumps, segment register reloads.
6. `src/stack_dump.c` — how `call`/`push` actually write memory, little-endian.
7. `src/kmain.c` — everything wired together.
8. Bonus: `src/keyboard.c` + `src/shell.c` — PS/2 polling, command dispatch.

### Interactive learning workbook

Open `learn.html` in a browser. It contains 14 modules and 69 multiple-choice questions.
Each lesson includes code examples, paper exercises, and defense prompts.

The file works offline. It uses no external fonts, libraries, or images.

The notebook saves answers and notes when browser storage works.
If saving fails, export your notes before you close the page.
Before you change browsers or move the file, export your notes.

## 3. Build & run

```sh
# toolchain (macOS, Homebrew) — nasm you may already have
brew install i686-elf-gcc nasm qemu            # + docker for the ISO

make            # kernel.elf
make run        # QEMU, boots the ELF directly (fast dev loop)
make iso        # bootable GRUB image kfs2.iso (needs docker)
make run-iso    # boot the ISO (what the evaluation will do)
```

Headless check (all output is mirrored to COM1):

```sh
qemu-system-i386 -kernel kernel.elf -display none -serial stdio
```

Expected: boot banner → GDT table summary → stack hexdump with `ESP` marked
→ `kfs2> ` prompt.

## 4. Verification checklist (subject's grading points)

- [ ] GDT located at `0x00000800` — see the printed base in the GDT summary.
- [ ] GDT holds exactly: kernel code/data/stack + user code/data/stack.
- [ ] GDT declared to the CPU: `lgdt` in `gdt_flush.asm` + selector reloads.
- [ ] Kernel stack: 16 KiB reserved in `boot.asm`, ESP set before any C runs.
- [ ] Stack printed human-friendly: address / hex / ASCII, ESP/EBP marked.
- [ ] `make iso` image boots via GRUB (`make run-iso`).
- [ ] Repo ≤ 10 MB.

## 5. Resources (quality, curated)

**Core references for this project:**
- OSDev wiki — [GDT Tutorial](https://wiki.osdev.org/GDT_Tutorial): the exact
  structures this project builds.
- OSDev wiki — [Bare Bones](https://wiki.osdev.org/Bare_Bones) and
  [Meaty Skeleton](https://wiki.osdev.org/Meaty_Skeleton): minimal boot and
  freestanding code organization.
- OSDev wiki — [Multiboot](https://wiki.osdev.org/Multiboot) /
  [GRUB](https://wiki.osdev.org/GRUB): the boot protocol in boot.asm.
- Intel SDM Vol. 3, chapters 3 & 5 —
  [intel.com/sdm](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html):
  authoritative segmentation text (descriptor figures match gdt.c field-by-field).

**For the bonus / deeper drills:**
- OSDev wiki — [PS/2 Keyboard](https://wiki.osdev.org/PS/2_Keyboard) and
  ["8042" Controller](https://wiki.osdev.org/%228042%22_PS/2_Controller).
- OSDev wiki — [Serial Ports](https://wiki.osdev.org/Serial_Ports) (COM1 mirror).
- OSDev wiki — [VGA Hardware](https://wiki.osdev.org/VGA_Hardware) (text mode).

**Long-form tutorials (read one, not all):**
- Nick Blundell, *Writing a Simple Operating System from Scratch* (free PDF,
  search the title) — the best pedagogical boot-to-shell walkthrough.
- [littleosbook](https://littleosbook.github.io/) — short, GDT-focused.
- [os-tutorial (cfenollosa)](https://github.com/cfenollosa/os-tutorial) —
  step-by-step from BIOS boot to protected mode.

**Tool docs:** [NASM manual](https://www.nasm.us/xdoc/2.16.01/html/nasmdoc0.html),
GNU `ld` linker scripts (see the comment blocks in `linker.ld` itself).