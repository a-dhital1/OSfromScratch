CC = i686-elf-gcc
AS = nasm
LD = i686-elf-gcc

CFLAGS = -ffreestanding -O2 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -T link.ld -nostdlib

OBJECTS = boot.o kmain.o

all: kernel.elf

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJECTS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

run: kernel.elf
	qemu-system-i386 -kernel kernel.elf

clean:
	rm -f *.o kernel.elf
