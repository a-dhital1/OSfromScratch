global loader
extern kmain

section .text
loader:
	cld
	and rsp, -16
	call kmain

.hang:
	cli
	hlt
	jmp .hang
