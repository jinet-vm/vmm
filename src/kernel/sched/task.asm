format elf64

public tasking_enter

section '.text' executable
use64

; keep same with task.c
struc task
{
	.rrax dq ?
	.rrcx dq ?
	.rrdx dq ?
	.rrbx dq ?
	.rrsp dq ?
	.rrbp dq ?
	.rrsi dq ?
	.rrdi dq ?
	.rrip dq ?
	.rr8 dq ?
	.rr9 dq ?
	.rr10 dq ?
	.rr11 dq ?
	.rr12 dq ?
	.rr13 dq ?
	.rr14 dq ?
	.rr15 dq ?
	.name dq ?
	.pid dq ?
	.next dq ?
}

extrn curTask

.final_rrip: dq 0

tasking_enter:
	mov rax, curTask
	mov rax, [rax]
	virtual at rax
		ctask task
	end virtual
	mov rsp, [ctask.rrsp]
	jmp [ctask.rrip]
	; mov [.final_rrip], QWORD [ctask.rrip]
	mov rcx, [ctask.rrcx]
	mov rdx, [ctask.rrdx]
	mov rbx, [ctask.rrbx]
	mov rsp, [ctask.rrsp]
	mov rbp, [ctask.rrbp]
	mov rsi, [ctask.rrsi]
	mov rdi, [ctask.rrdi]
	mov r8, [ctask.rr8]
	mov r9, [ctask.rr9]
	mov r10, [ctask.rr10]
	mov r11, [ctask.rr11]
	mov r12, [ctask.rr12]
	mov r13, [ctask.rr13]
	mov r14, [ctask.rr14]
	mov r15, [ctask.rr15]
	mov rax, [ctask.rrax]
	jmp [ctask.rrip] ; or jmp far?