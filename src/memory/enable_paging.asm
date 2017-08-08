format elf64

;include 'inc/macro.inc'
;include 'inc/procedures.inc'

section '.text' executable
use64

public enable_paging

enable_paging:;(void * PD) ; ret, cause low memory mapped onto itself
	; push rbp
	; cli
	; mov ebp, rsp
	; mov eax, [ebp+8]
	; mov cr3, rax
	; mov eax, cr0
	; or eax, 0x80000000
	; mov cr0, rax
	; pop rbp
	; todo: delete it and use inline asm
	ret