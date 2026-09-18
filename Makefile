CC = i686-elf-gcc
AS = nasm
LD = i686-elf-gcc

CFLAGS = -ffreestanding -O2 -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -T link.ld -nostdlib

OBJECTS = boot.o kmain.o

all: os.iso

os.iso: kernel.elf iso/boot/grub/menu.lst
	cp kernel.elf iso/boot/
	xorrisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -c boot/grub/boot.catalog -o os.iso iso

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJECTS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: os.iso
	qemu-system-i386 -cdrom os.iso

clean:
	rm -f *.o kernel.elf os.iso iso/boot/kernel.elf iso/boot/grub/boot.catalog
