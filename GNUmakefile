# Nuke built-in rules.
.SUFFIXES:

# Delete the target of a failed recipe.
.DELETE_ON_ERROR:

# This is the name that our final executable will have.
override OUTPUT := myos

# User controllable toolchain and toolchain prefix.
TOOLCHAIN :=
TOOLCHAIN_PREFIX :=
ifneq ($(TOOLCHAIN),)
        ifeq ($(TOOLCHAIN_PREFIX),)
                TOOLCHAIN_PREFIX := $(TOOLCHAIN)-
        endif
endif

# User controllable C compiler command.
ifneq ($(TOOLCHAIN_PREFIX),)
        CC := $(TOOLCHAIN_PREFIX)gcc
else
        CC := cc
endif

# User controllable linker command.
LD := $(TOOLCHAIN_PREFIX)ld

# Defaults ovverides for variables if using "llvm" as toolchain.
ifeq ($(TOOLCHAIN),llvm)
        CC := clang
        LD := ld.lld
endif

# User controllable C flags.
CFLAGS := -g -O2 -pipe

# User controllable nasm flags.
NASMFLAGS := -g

# User controllable linker flags. We set none by default.
LDFLAGS := 

# Check if CC is Clang.
override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep -q '^Target: '; echo $$?)

# If the C compiler is Clang, set the target as needed.
ifeq ($(CC_IS_CLANG), 1)
        override CC += \
                -target x86_64-unknown-none-elf
endif

# Internal C flags that should not be changed by the user.
override CFLAGS += \
        -Wall \
        -Wextra \
        -std=gnu11 \
        -ffreestanding \
        -fno-stack-protector \
        -fno-stack-check \
        -fno-lto \
        -fno-PIC \
        -ffunction-sections \
        -fdata-sections \
        -m64 \
        -march=x86-64 \
        -mabi=sysv \
        -mno-80387 \
        -mno-mmx \
        -mno-sse \
        -mno-sse2 \
        -mno-red-zone \
        -mcmodel=kernel

# Internal C preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
        -I src \
        $(CPPFLAGS) \
        -MMD \
        -MP

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS := \
        -f elf64 \
        $(patsubst -g, -g -F dwarf,$(NASMFLAGS)) \
        -i src/ \
        -Wall

# Internal linker flags that should not be changed by the user.
override LDFLAGS += \
        -m elf_x86_64 \
        -nostdlib \
        -static \
        -z max-page-size=0x1000 \
        -z noexecstack \
        --gc-sections \
        -T linker.lds

# Check if the linker supports -no-pie and enable it if it does
override LD_HAS_NO_PIE := $(shell ! $(LD) --help 2>/dev/null | grep -qE '(^|[[:space:]])--?no-pie([[:space:]]|$$)'; echo $$?)

ifeq ($(LD_HAS_NO_PIE),1)
        override LDFLAGS += \
                -no-pie
endif

# Use "find" to glob all *c, *.S, and *asm files in the tree and obtain the
# object and header dependency file names.
override SRCFILES := $(shell find -L src -type f 2>/dev/null | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override NASMFILES := $(filter %.asm,$(SRCFILES))
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d) $(NASMFILES:.asm=.asm.d))

# Default target. This must come first, before header dependencies.
.PHONY: all
all: bin/$(OUTPUT)

# Include header dependencies.
-include $(HEADER_DEPS)

# Link rules for the final executable.
bin/$(OUTPUT): GNUmakefile linker.lds $(OBJ)
	mkdir -p "$(dir $@)"
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Compilation rules for *.c files.
obj/%.c.o: %.c GNUmakefile
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.S files.
obj/%.S.o: %.S GNUmakefile
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.asm (nasm) files.
obj/%.asm.o: %.asm GNUmakefile
	mkdir -p "$(dir $@)"
	nasm $(NASMFLAGS) -MD $(@:.o=.d) -MP $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf bin obj


# Use the x86-64 cross compiler on macOS.
# Only set the default if it wasn't supplied on the command line.
TOOLCHAIN_PREFIX ?= x86_64-elf-

# Set the tools explicitly for macOS.
CC := x86_64-elf-gcc
LD := x86_64-elf-ld

# Build the bootable ISO.
.PHONY: iso run

iso: bin/myos
	@echo "Preparing ISO directory..."
	mkdir -p iso_root/boot/limine
	mkdir -p iso_root/EFI/BOOT

	cp bin/myos iso_root/boot/myos

	cp limine.conf \
	   limine-binary/limine-bios.sys \
	   limine-binary/limine-bios-cd.bin \
	   limine-binary/limine-uefi-cd.bin \
	   iso_root/boot/limine/

	cp limine-binary/BOOTX64.EFI \
	   limine-binary/BOOTIA32.EFI \
	   iso_root/EFI/BOOT/

	@echo "Creating bootable ISO..."
	xorriso -as mkisofs \
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
	   iso_root -o image.iso

	./limine-binary/limine bios-install image.iso

# Build and boot in QEMU.
run: iso
	qemu-system-x86_64 \
	   -m 256M \
	   -cdrom image.iso \
	   -boot d \
	   -no-reboot

