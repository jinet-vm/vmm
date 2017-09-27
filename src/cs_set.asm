format ELF64
use64
public cs_set

section '.text' executable

cs_set:
	mov rax, alpha
	push rax
	push rdi
	retf
	alpha:
	ret