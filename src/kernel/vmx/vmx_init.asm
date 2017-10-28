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

public gr

gr:
_rax dq 0
_rsi dq 0
_rcx dq 0
_rbx dq 0

public vmx_return
extrn virt_exit
vmx_return:
	mov [_rax], rax
	mov [_rsi], rsi
	mov [_rcx], rcx
	mov [_rbx], rbx
	call virt_exit
	mov rax, [_rax]
	mov rsi, [_rsi]
	mov rcx, [_rcx]
	mov rbx, [_rbx]
	vmresume