global loader                   ; Make entry point visible to linker
extern kmain                    ; Will be used in Chapter 4

MAGIC_NUMBER equ 0x1BADB002     ; Multiboot 1 Magic Number standard
FLAGS        equ 0x0            ; Clean flags (no extra memory features required yet)
CHECKSUM     equ -MAGIC_NUMBER  ; Calculate checksum

section .text
align 4
    dd MAGIC_NUMBER             ; Embed multiboot signature
    dd FLAGS
    dd CHECKSUM

loader:
    mov esp, kernel_stack + 4096 ; Set up your C execution stack boundary

    mov eax, 0xCAFEBABE         ; Lock your custom verification signature into RAX

.loop:
    jmp .loop                   ; Infinite loop keeping CPU pinned

section .bss
align 4
kernel_stack:
    resb 4096                   ; Allocate 4KB of uninitialized stack memory

