
# Cross-compiler and assembler
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-gcc

# Freestanding x86_64 kernel flags
CFLAGS = -ffreestanding -O0 -Wall -Wextra \
	 -fno-stack-protector -fno-builtin -g \
	 -m64 -mno-red-zone -mcmodel=large

ASFLAGS = -f elf64

# Linker flags
LDFLAGS = -T link.ld -nostdlib

# QEMU configuration
QEMU = qemu-system-x86_64
QEMUFLAGS = -cpu max -m 512M -machine pc \
	    -cdrom os.iso -boot d \
	    -serial stdio

# Kernel object files
OBJECTS = boot.o kmain.o

# Files included in the ISO
ISO_BOOT_FILES = iso/limine-bios-cd.bin \
		 iso/limine-uefi-cd.bin \
		 iso/limine-bios.sys \
		 iso/limine.conf

.PHONY: all run debug clean

# Default target
all: os.iso


# Build the bootable ISO
os.iso: kernel.elf $(ISO_BOOT_FILES)
	cp kernel.elf iso/boot/kernel.elf
	xorriso -as mkisofs -R \
		-b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image \
		--protective-msdos-label \
		iso -o $@
	limine bios-install $@

# Link the kernel
kernel.elf: $(OBJECTS) link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Assemble the NASM source
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Compile C source
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the OS in QEMU
run: os.iso
	$(QEMU) $(QEMUFLAGS)

# Debug with LLDB/GDB remote protocol
debug: os.iso
	$(QEMU) -machine pc,accel=tcg -m 512M \
		-cdrom os.iso -boot d \
		-serial stdio -s -S

# Remove generated files
clean:
	rm -f *.o kernel.elf os.iso \
		iso/boot/kernel.elf serial.log
