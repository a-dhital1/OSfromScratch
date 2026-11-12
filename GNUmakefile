.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

# Kernel filename
KERNEL := myos

# Compiler tools for macOS cross-compiling
TOOLCHAIN_PREFIX ?= x86_64-elf-
CC := $(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PREFIX)ld

# Other tools
XORRISO := xorriso
QEMU := qemu-system-x86_64

# Directories
BUILD_DIR := obj
BIN_DIR := bin
SYSROOT := sysroot
ISO_ROOT := iso_root
LIMINE_DIR := limine-binary

# Locate C sources
SOURCES := $(shell find kernel libk -name '*.c' -type f | LC_ALL=C sort)

# Convert source filenames into object filenames
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Dependency files
DEPS := $(OBJECTS:.o=.d)

# Compiler flags
CFLAGS := -std=gnu11 -O2 -g \
          -Wall -Wextra \
          -ffreestanding \
          -fno-stack-protector \
          -fno-stack-check \
          -fno-lto \
          -fno-PIC \
          -fno-builtin \
          -ffunction-sections \
          -fdata-sections \
          -m64 \
          -march=x86-64 \
          -mabi=sysv \
          -mno-red-zone \
          -mno-80387 \
          -mno-mmx \
          -mno-sse \
          -mno-sse2 \
          -mcmodel=kernel \
          -MMD -MP

# Header paths
CPPFLAGS := -Ikernel \
            -Ikernel/include \
            -Ilibk/include

# Linker flags
LDFLAGS := -m elf_x86_64 \
           -nostdlib \
           -static \
           -z max-page-size=0x1000 \
           -z noexecstack \
           --gc-sections \
           -no-pie \
           -T linker.lds

.PHONY: all clean install iso run

# Build the kernel
all: $(BIN_DIR)/$(KERNEL)

# Compile individual C files
$(BUILD_DIR)/%.o: %.c GNUmakefile
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Link all object files
$(BIN_DIR)/$(KERNEL): $(OBJECTS) linker.lds GNUmakefile
	mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Install the kernel and headers into sysroot
install: all
	mkdir -p $(SYSROOT)/boot
	mkdir -p $(SYSROOT)/usr/include/kernel
	mkdir -p $(SYSROOT)/usr/include/libk

	cp $(BIN_DIR)/$(KERNEL) $(SYSROOT)/boot/
	cp kernel/include/kernel/*.h $(SYSROOT)/usr/include/kernel/
	cp libk/include/*.h $(SYSROOT)/usr/include/libk/

# Package the ISO
iso: install
	mkdir -p $(ISO_ROOT)/boot/limine
	mkdir -p $(ISO_ROOT)/EFI/BOOT

	cp $(SYSROOT)/boot/$(KERNEL) $(ISO_ROOT)/boot/

	cp limine.conf $(ISO_ROOT)/boot/limine/
	cp $(LIMINE_DIR)/limine-bios.sys $(ISO_ROOT)/boot/limine/
	cp $(LIMINE_DIR)/limine-bios-cd.bin $(ISO_ROOT)/boot/limine/
	cp $(LIMINE_DIR)/limine-uefi-cd.bin $(ISO_ROOT)/boot/limine/

	cp $(LIMINE_DIR)/BOOTX64.EFI $(ISO_ROOT)/EFI/BOOT/
	cp $(LIMINE_DIR)/BOOTIA32.EFI $(ISO_ROOT)/EFI/BOOT/

	$(XORRISO) -as mkisofs \
		-R -r -J \
                -b boot/limine/limine-bios-cd.bin \
                -no-emul-boot \
                -boot-load-size 4 \
                -boot-info-table \
                -hfsplus \
                -apm-block-size 2048 \
                --efi-boot boot/limine/limine-uefi-cd.bin \
                -efi-boot-part \
                --efi-boot-image \
                --protective-msdos-label \
                $(ISO_ROOT) -o image.iso

	./$(LIMINE_DIR)/limine bios-install image.iso

# Launch the OS
run: iso
	$(QEMU) -m 256M \
                -cdrom image.iso \
                -boot d \
                -no-reboot

# Clean generated build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) \
               $(SYSROOT) $(ISO_ROOT) image.iso

# Include generated header dependencies
-include $(DEPS)
