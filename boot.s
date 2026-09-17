; boot.s
global leader
extern kmain

MAGIC_NUMBER equ 0x1BADB002
FLAGS        equ 0x00000003
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .multiboot
align 4
	dd MAGIC_NUMBER
	dd FLAGS
	dd CHECKSUM

section .text
loader:
	mov esp, kernel_stack + 4096

	mov eax, 0
	cpuid

	mov [0xB8000], ebx
	mov [0xB8008], edx
	mov [0xB8010], ecx
	
	
	call kmain

.loop:
	jmp .loop

section .bss
align 4
kernel_stack:
	resb 4096
