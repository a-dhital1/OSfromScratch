CC = i686-elf-gcc
AS = nasm
LD = i686-elf-gcc

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector -fno-builtin
ASFLAGS = -f elf32
LDFLAGS = -m32 -T link.ld -nostdlib

QEMU = qemu-system-x86_64
QEMUFLAGS = -cpu max -m 8G -machine pc -cdrom os.iso -boot d -serial file:serial.log

OBJECTS = boot.o kmain.o

all: os.iso

os.iso: kernel.elf iso/limine.cfg
	cp kernel.elf iso/boot/
	xorriso -as mkisofs -R -r -J -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso -o os.iso
	limine bios-install os.iso


kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJECTS)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: os.iso
	$(QEMU) $(QEMUFLAGS)

clean:
	rm -f *.o kernel.elf os.iso iso/boot/kernel.elf serial.log
