; boot.s
global leader
extern kmain
extern sum_of_three

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

	push dword 3
	push dword 2
	push dword 1	
	
	call sum_of_three

.loop:
	jmp .loop

section .bss
align 4
kernel_stack:
	resb 4096
