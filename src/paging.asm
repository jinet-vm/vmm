format ELF

include 'inc/macro.inc'
include 'inc/procedures.inc'

section '.text' executable
use32

public enable_paging

enable_paging:;(void * PD) ; ret, cause low memory mapped onto itself
	push ebp
	cli
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr3, eax
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	pop ebp
	ret