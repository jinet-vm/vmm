format ELF64
use64
public cs_set

section '.text' executable

cs_set:
	mov rax, alpha
	push rdi
	push rax
	retf
	alpha:
	ret