format ELF64

section '.text' executable
Use64

public vmx_check
vmx_check: ; bool vmx_check();
	mov eax, 1
	cpuid
	bt ecx, 5
	jnc .nosupport
	; supported!
	mov rax, 1
	ret
	.nosupport:
	mov rax, 0
	ret