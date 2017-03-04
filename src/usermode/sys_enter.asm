format ELF

section '.text' executable
use32

public sys_enter
sys_enter:
	pushad
	mov ecx, esp
	mov edx, .back
	sysenter
	.back:
	popad
	ret

public sys_write
sys_write: ; (char * buf) // null-terminated
	push ebp
	mov ebp, esp
	push esi
	mov esi, [ebp+8]
	mov eax, 1
	call sys_enter
	pop esi
	pop ebp
	ret