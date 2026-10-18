all:
	i686-elf-gcc -m32 -march=i386 -ffreestanding -nostdlib -nodefaultlibs -fno-builtin -fno-stack-protector -c kernel.c -o kernel.o

clean:
	rm kernel.o


show_headers :
	i686-elf-objdump -h kernel.o
	# "TABLE OF CONTENTS"
	#  ```text
	# Idx Name          Size      VMA       File off  Algn
	#   0 .text         00000018  00000000  00000034  2**0
	#   1 .data         00000000  00000000  0000004c
	#   2 .bss          00000000  00000000  0000004c
	#  ```
	#
	#  Reading it:
	#
	#  - .text size 0x18 = 24 bytes of machine code. That's exactly the 24 bytes we dissembled (0:
	#    through 17:). ✓
	#  - .data = 0 bytes — initialized global variables. You have none.
	#  - .bss = 0 bytes — uninitialized globals. Also none.
	#  - VMA all zeros — "virtual memory address." A .o doesn't know yet where in memory it will
	#    live. The linker decides that later — this is exactly what linker.ld is for. Forward
	#    connection: our linker script says .text goes at 1 MiB.

show_dissassemble: # decodes into readable assembly
	i686-elf-objdump -d kernel.o
