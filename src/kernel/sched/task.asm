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
	.rrflags dq ?
	.name dq ?
	.pid dq ?
	.next dq ?
}

extrn curTask


; ok, here's the magic:
; we modify registers in IST (spec. RIP, RSP, RFLAGS - see SDM-3B: p201=6.14.1)
; so that they would align with the curTask
; and then we iretq

task_load: ; ALMOST load
	mov rax, curTask
	mov rax, [rax]
	virtual at rax
		.ctask task
	end virtual
	mov rdx, [.ctask.rrdx]
	mov rbx, [.ctask.rrbx]
	mov rsp, [.ctask.rrsp]
	mov rbp, [.ctask.rrbp]
	mov rsi, [.ctask.rrsi]
	mov rdi, [.ctask.rrdi]
	mov r8, [.ctask.rr8]
	mov r9, [.ctask.rr9]
	mov r10, [.ctask.rr10]
	mov r11, [.ctask.rr11]
	mov r12, [.ctask.rr12]
	mov r13, [.ctask.rr13]
	mov r14, [.ctask.rr14]
	mov r15, [.ctask.rr15]
	
	mov rcx, [.ctask.rrax]
	mov [.final_rrax], rcx ; it's loaded during task_load

	mov rcx, [.ctask.rrip]
	mov [task_load_rip], rcx

	mov rcx, [.ctask.rrsp]
	mov [task_load_rsp], rcx

	mov rcx, [.ctask.rrflags]
	mov [task_load_rflags], rcx

	mov rcx, [.ctask.rrcx]

	mov rax, [.final_rrax]
	ret
	.final_rrax: dq 0

task_load_rip: dq 0
task_load_rsp: dq 0
task_load_rflags: dq 0

tasking_enter:
	mov rax, curTask
	mov rax, [rax]
	virtual at rax
		.ctask task
	end virtual

	mov rcx, [.ctask.rrflags]
	push rcx
	popf

	mov rsp, [.ctask.rrsp]
	mov rdx, [.ctask.rrdx]
	mov rbx, [.ctask.rrbx]
	mov rsp, [.ctask.rrsp]
	mov rbp, [.ctask.rrbp]
	mov rsi, [.ctask.rrsi]
	mov rdi, [.ctask.rrdi]
	mov r8, [.ctask.rr8]
	mov r9, [.ctask.rr9]
	mov r10, [.ctask.rr10]
	mov r11, [.ctask.rr11]
	mov r12, [.ctask.rr12]
	mov r13, [.ctask.rr13]
	mov r14, [.ctask.rr14]
	mov r15, [.ctask.rr15]
	
	mov rcx, [.ctask.rrax]
	mov [.final_rrax], rcx
	mov rcx, [.ctask.rrip]
	mov [.final_rrip], rcx
	mov rcx, [.ctask.rrcx]

	mov rax, [.final_rrax]
	jmp qword [.final_rrip] ; or jmp far?

	.final_rrip: dq 0
	.final_rrax: dq 0


; so we want to save the the task
; 1. timer irq gets RIP, RSP, RFLAGS from stack
; 2. places it to task_save_{rip,rsp,rflags}
; 3. calls task_save (daily reminder: DON'T TOUCH ANYTHING BEFORE THAT)

public task_save_rip
public task_save_rsp
public task_save_rflags
task_save_rip: dq 0
task_save_rsp: dq 0
task_save_rflags: dq 0

task_save:
	mov [.task_rrax], rax
	mov rax, curTask
	mov rax, [rax]
	virtual at rax
		.ctask task
	end virtual

	mov [.ctask.rrcx], rcx
	
	mov rcx, [task_save_rsp]
	mov [.ctask.rrsp], rcx

	mov rcx, [task_save_rip]
	mov [.ctask.rrip], rcx

	mov rcx, [task_save_rflags]
	mov [.ctask.rrflags], rcx

	mov [.ctask.rrdx], rdx
	mov [.ctask.rrbx], rbx
	mov [.ctask.rrsp], rsp
	mov [.ctask.rrbp], rbp
	mov [.ctask.rrsi], rsi
	mov [.ctask.rrdi], rdi
	mov [.ctask.rr8], r8
	mov [.ctask.rr9], r9
	mov [.ctask.rr10], r10
	mov [.ctask.rr11], r11
	mov [.ctask.rr12], r12
	mov [.ctask.rr13], r13
	mov [.ctask.rr14], r14
	mov [.ctask.rr15], r15

	mov rax, [.task_rrax]
	ret
	.task_rrax: dq 0s
	