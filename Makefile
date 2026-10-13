# ============================================================================
# Makefile -- KFS_2 build system (the teaching edition).
# ============================================================================
# PIPELINE
# --------
#   src/*.c    --i686-elf-gcc-->  build/*.o    (32-bit freestanding objects)
#   src/*.asm  --nasm -f elf32--> build/*.o    (32-bit ELF objects)
#   build/*    --i686-elf-ld + linker.ld--> kernel.elf   (the loadable image)
#
#   `make`      builds kernel.elf  (the default goal)
#   `make run`  boots it in QEMU without any disk: GRUB ships inside the
#               QEMU machine firmware and loads an ELF via -kernel
#   `make iso`  wraps kernel.elf in a bootable CD image kfs2.iso using
#               grub-mkrescue inside the prebuilt `grub-builder` container
#               (build the image first with `make img-docker`)
#   `make run-iso` boots that CD in QEMU, exercising the REAL GRUB path
#               (menu -> multiboot -> kernel) instead of the -kernel shortcut
#   `make clean` deletes build/, kernel.elf and the isodir/ staging tree.
#               kfs2.iso is DELIBERATELY KEPT: the bootable image is one of
#               the three deliverables of the project (source, Makefile,
#               image), so `make clean` must not destroy a good one.
#
# GIT: kernel.elf and the whole build/ tree are gitignored; the deliverable
#      image is kfs2.iso (small -- the kernel does nothing visual, so it
#      stays far below the 10 MB repo ceiling).
# ============================================================================

# ---------------------------------------------------------------------------
# THE CROSS TOOLCHAIN (all from Homebrew's gcc cross "elf" toolset).
#   i686-elf-gcc : GNU C compiler targeting 32-bit x86 WITHOUT a host OS
#                  or libc -- the ONLY compiler that can build a kernel
#                  from a macOS arm64 host.
#   nasm         : the x86 assembler; -f elf32 makes it emit 32-bit ELF
#                  objects compatible with i686-elf-gcc's output.
#   i686-elf-ld  : the linker; fed OUR linker.ld, it produces the final
#                  image layout. Using this instead of the host `ld` is
#                  the "never use a host linker script" rule from
#                  AGENTS.md made into one token.
# ---------------------------------------------------------------------------
CC  := i686-elf-gcc
AS  := nasm
LD  := i686-elf-ld

# ---------------------------------------------------------------------------
# CFLAGS -- each flag is load-bearing. Omitting ANY of them breaks the
# build or the boot in a specific, boring way:
#
#   -m32 : emit 32-bit objects. The host gcc on an Apple-Silicon Mac is an
#          aarch64 compiler; without -m32 you get ARM instructions and
#          nothing the i386 target understands.
#   -march=i386 : target the bare i386 (486-class) ISA. Without it the
#          cross gcc defaults to the i686 baseline, which permits SSE2
#          instructions; the QEMU "i386" machine emulates a CPU with no
#          SSE2, so such an instruction faults with #UD (invalid opcode)
#          and the kernel dies mid-kmain. The subject mandates i386.
#   -ffreestanding : tell the compiler there is no host OS underneath:
#          do not assume malloc/printf/sbrk exist, do not elide code it
#          believes is "only visible to the library", do not assume
#          memory-mapped I/O is volatile. Without it the compiler may
#          optimize away device writes or inline calls to functions that
#          do not exist in the kernel.
#   -nostdlib : link NO startup code (no crt0.o, no libc init). A kernel
#          has no process to "start"; entering libc's __libc_start_main
#          on bare metal would be nonsense.
#   -nodefaultlibs : link NO default libraries (-lc -lgcc -lmath...).
#          Together with -nostdlib this is "the binary stands on its own".
#   -fno-builtin : disable builtin-function optimization. Without it the
#          compiler may replace calls it recognizes (memcpy, strlen, ...)
#          with its own inlined versions, and may delete loops it thinks
#          "can't have side effects" -- e.g. an input-polling loop that
#          reads a port. In a kernel, every "optimization" it cannot see
#          is a device write.
#   -fno-stack-protector : -fstack-protector is ON by default; without
#          this flag each function references __stack_chk_guard and
#          calls __stack_chk_fail() -- symbols that do not exist in a
#          freestanding link: "undefined reference" and a dead build.
#   -fno-exceptions : no C++ exception-unwind machinery (a no-op for C
#          today, but it keeps the objects honest if the project ever
#          mixes in C++; a kernel never unwinds anyway).
#   -fno-rtti : no C++ runtime type information (same reason: surface
#          area stays zero).
#   -fno-asynchronous-unwind-tables : do not emit .eh_frame data. A
#          kernel has no unwinder, so .eh_frame is dead weight; our
#          linker.ld also /DISCARD/s it as a safety net, but the first
#          line of defence is not asking the compiler for it at all.
#   -Wall -Wextra : maximum compiler scrutiny. In user space a warning
#          is a warning; in a kernel a warning is a "the screen went
#          black" bug, so we compile the strictest configuration the
#          compiler offers us and treat every warning as an error in
#          waiting to happen.
# ---------------------------------------------------------------------------
CFLAGS := -m32 -march=i386 -ffreestanding -nostdlib -nodefaultlibs
CFLAGS += -fno-builtin -fno-stack-protector -fno-exceptions -fno-rtti
CFLAGS += -fno-asynchronous-unwind-tables -Wall -Wextra

# ASFLAGS: `-f elf32` tells nasm the OUTPUT FORMAT is 32-bit ELF. Without
# it, nasm's default target (usually "intel syntax for the host") would
# emit an object that i686-elf-ld refuses ("file not recognized" or a
# relocation-type mismatch). It is the one flag that says "this .asm is
# 32-bit, for an ELF link".
ASFLAGS := -f elf32

# LDFLAGS:
#   -T linker.ld : use OUR script, not the linker's default one. This is
#          where the 1 MiB origin, the multiboot-first ordering, the
#          .bss layout and the /DISCARD/ rules actually live (see the
#          comments in linker.ld for why each one is load-bearing).
#   -nostdlib    : the ld-side twin of the -nostdlib/-nodefaultlibs above:
#          link no CRT and no default libraries into kernel.elf.
LDFLAGS := -T linker.ld -nostdlib

# ---------------------------------------------------------------------------
# OBJECT DISCOVERY: every src/*.c and src/*.asm becomes a build/*.o.
# `$(wildcard ...)` + the `src/%.ext=build/%.o` substitution keeps the
# list self-maintaining: dropping a new .c or .asm into src/ needs no
# Makefile change -- this is why the wildcard matters and why the rules
# below are PATTERN rules (build/%.o: src/%.c), not per-file listings.
# ---------------------------------------------------------------------------
SRC_C   := $(wildcard src/*.c)
SRC_ASM := $(wildcard src/*.asm)
OBJ     := $(SRC_C:src/%.c=build/%.o) $(SRC_ASM:src/%.asm=build/%.o)

# All fake targets declared up front; `make -p` users (and our own
# memory) can see the full command surface at a glance.
.PHONY: all run iso run-iso img-docker clean

# ---------------------------------------------------------------------------
# THE DEFAULT GOAL: `make` == `make all` == a fresh kernel.elf.
# `| build` is an ORDER-ONLY prerequisite: make creates the build/
# directory before compiling anything (without claiming the directory's
# mtime triggers a rebuild -- that is what the vertical bar means).
# kernel.elf also depends on linker.ld: changing the script must force a
# re-link, because the linker script is an input to linking, not to
# compiling.
# ---------------------------------------------------------------------------
all: kernel.elf

kernel.elf: $(OBJ) linker.ld | build
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Pattern rule: C -> build/%.o. One recipe handles every src/*.c.
build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule: assembly -> build/%.o. nasm syntax is -o OUT IN (the
# opposite order of gcc's -c IN -o OUT; a classic "which came first" bug
# when you mix them in a Makefile).
build/%.o: src/%.asm | build
	$(AS) $(ASFLAGS) -o $@ $<

# The build/ directory itself.
build:
	mkdir -p build

# ---------------------------------------------------------------------------
# DEVELOPMENT BOOT: QEMU's built-in (QEMU-as-a-bootloader) GRUB loads
# the ELF directly, no disk image needed. This is the fast inner loop:
#
#   qemu-system-i386 -kernel kernel.elf -serial stdio
#
#   -kernel kernel.elf : QEMU starts its bundled GRUB, which reads the
#          multiboot header from our ELF and jumps into _start exactly
#          as a real GRUB on a CD would. No image, no waiting.
#   -serial stdio      : wire the emulated COM1 (0x3F8, 115200 8N1) to
#          this terminal. The kernel's printk mirrors every line to both
#          the VGA screen AND COM1 (see src/printk.c), so in a headless
#          terminal we see the whole boot in plain text; -nographic
#          is not needed because we only need the serial, not a VNC
#          window. Ctrl-A then X (or Ctrl-C in -nographic mode) exits.
# ---------------------------------------------------------------------------
run: kernel.elf
	qemu-system-i386 -kernel kernel.elf -serial stdio

# ---------------------------------------------------------------------------
# THE DELIVERABLE IMAGE: a real bootable CD with real GRUB on it.
# The staging layout grub-mkrescue expects:
#
#   isodir/boot/grub/grub.cfg   <-- the only file we write by hand
#   isodir/boot/kernel.elf      <-- a copy of our kernel, placed where
#                                    the menuentry's `multiboot` path says
#
#   grub.cfg is minimal on purpose:
#       set timeout=0          boot the first (only) entry immediately;
#                               no 5-second menu stare, no keypress
#                               needed -- the image just starts the
#                               kernel, which is what the grader wants.
#       menuentry "KFS_2" {
#           multiboot /boot/kernel.elf
#       }
#       `multiboot` is GRUB's own multiboot loader module: it scans the
#       file for 0x1BADB002 and performs the same contract boot.asm's
#       header promised.
#
# WHY A DOCKER CONTAINER, AND WHY --platform linux/amd64 ON macOS?
#   grub-mkrescue is a shell wrapper that needs xorriso (writes ISOs),
#   mtools (writes the FAT partition table into the ISO) and the
#   grub-pc module tree -- and Debian/Ubuntu's `grub-pc-bin` (the
#   package that carries the i386-pc GRUB modules) is shipped for the
#   amd64 architecture ONLY: it contains prebuilt stage2s and modules
#   for the PC BIOS (i386) target. On an Apple-Silicon Mac there is no
#   arm64 formula that provides them, so we run an amd64 container
#   (Ubuntu + grub-pc-bin + xorriso + mtools) and, because the host CPU
#   is arm64, we force `--platform linux/amd64` so Docker runs that
#   amd64 image under Rosetta. The repo is mounted read-write at /build
#   inside the container, so `grub-mkrescue` writes kfs2.iso straight
#   into our working directory and the image is usable immediately.
# ---------------------------------------------------------------------------
iso: kfs2.iso

kfs2.iso: isodir/boot/grub/grub.cfg isodir/boot/kernel.elf img-docker
	docker run --rm --platform linux/amd64 -v $(PWD):/build -w /build \
		grub-builder grub-mkrescue -o kfs2.iso isodir

# The staging tree. Rebuilding either file forces the ISO to rebuild too,
# because a stale kfs2.iso is worse than no ISO.
isodir/boot/grub/grub.cfg: kernel.elf
	mkdir -p isodir/boot/grub
	printf 'set timeout=0\nmenuentry "KFS_2" {\n  multiboot /boot/kernel.elf\n}\n' > $@

isodir/boot/kernel.elf: kernel.elf
	mkdir -p isodir/boot
	cp kernel.elf $@

# Build (or refresh) the `grub-builder` image FROM AN INLINE Dockerfile:
# one printf, one docker build. The Dockerfile content is:
#
#   FROM ubuntu:24.04
#   ENV DEBIAN_FRONTEND=noninteractive
#   RUN apt-get update && apt-get install -y --no-install-recommends \
#         grub-pc-bin xorriso mtools ca-certificates \
#       && apt-get clean && rm -rf /var/lib/apt/lists/*
#
#   ubuntu:24.04         small, current, has all three packages.
#   DEBIAN_FRONTEND=noninteractive : grub-pc-bin runs a debconf preseed
#        script (which layout / which initrd to ship in the rescue image);
#        without noninteractive the build hangs waiting on a TTY.
#   grub-pc-bin          the amd64-only package with the PC BIOS GRUB
#        that the ISO actually boots from.
#   xorriso              writes the hybrid ISO/El Torito image.
#   mtools               writes the FAT partition (the "bootable" half of
#        the rescue image).
#   ca-certificates      apt needs them to talk to the mirrors; it also
#        keeps the image small vs a full desktop.
img-docker:
	printf '%s\n' \
		'FROM ubuntu:24.04' \
		'ENV DEBIAN_FRONTEND=noninteractive' \
		'RUN apt-get update && apt-get install -y --no-install-recommends grub-pc-bin xorriso mtools ca-certificates && apt-get clean && rm -rf /var/lib/apt/lists/*' \
		| docker build -t grub-builder -

# ---------------------------------------------------------------------------
# BOOT THE DELIVERABLE: the same QEMU flags as `run`, but the machine
# boots FROM the CD-ROM exactly like a bare PC would: firmware ->
# El Torito boot record on the CD -> GRUB stage1 -> stage2 -> our
# grub.cfg -> multiboot -> kernel. This is the regression test that
# "the image actually boots", which the -kernel shortcut does not
# exercise (the -kernel path bypasses CD firmware entirely).
# ---------------------------------------------------------------------------
run-iso: kfs2.iso
	qemu-system-i386 -cdrom kfs2.iso -serial stdio

# ---------------------------------------------------------------------------
# CLEAN: everything derivable. kfs2.iso is NOT deleted: it is a
# deliverable, not a build by-product (see the top-of-file note and the
# gitignore convention: build/ and kernel.elf are gitignored, kfs2.iso is
# the image the project ships).
# ---------------------------------------------------------------------------
clean:
	rm -rf build kernel.elf isodir
