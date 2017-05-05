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

public vmx_crinit
vmx_crinit: ; void vmx_crinit();
	mov rax, cr4
	bts rax, 13
	mov cr4, rax
	mov rax, cr0
	bts rax, 5
	mov cr0, rax
	ret

; todo: design a saner msr interface
public vmx_bmsr ; uint64_t vmx_bmsr();
vmx_bmsr:
	mov ecx, 0x480
	rdmsr ; rax contains the thing
	ret