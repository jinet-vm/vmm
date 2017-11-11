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


; todo: use STACK instead of .final_rrax-ish labels
; see tasking_enter for example

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
	mov [task_load_rrax], rcx ; it's loaded during task_load

	mov rcx, [.ctask.rrip]
	mov [task_load_rip], rcx

	mov rcx, [.ctask.rrsp]
	mov [task_load_rsp], rcx

	mov rcx, [.ctask.rrflags]
	mov [task_load_rflags], rcx

	mov rcx, [.ctask.rrcx]

	mov rax, [task_load_rrax]
	ret

task_load_rip: dq 0
task_load_rsp: dq 0
task_load_rflags: dq 0
task_load_rrax: dq 0

tasking_enter:
	xchg bx, bx
	call task_load
	push rcx
	mov rcx, [task_load_rflags]
	push rcx
	popf
	pop rcx
	mov rsp, [task_load_rsp]
	sti ; finally!
	jmp qword [task_load_rip]


; so we want to save the the task
; 1. timer irq gets RIP, RSP, RFLAGS from stack
; 2. places it to task_save_{rip,rsp,rflags}
; 3. calls task_save (daily reminder: DON'T TOUCH ANYTHING BEFORE THAT)

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
	.task_rrax: dq 0

task_switch: ; reminder: doesn't save RAX
	mov rax, curTask
	mov rax, [rax]
	virtual at rax
		.ctask task
	end virtual
	mov rcx, [.ctask.next]
	mov [curTask], rcx
	ret

public irq_sched
extrn lapic_eoi_send
; TODO: use consts for stack offsets?
irq_sched:
	cli
	xchg bx, bx
	push rcx ; +8 bytes on stack
	; it's complicated a bit
	; figure 6-8 from 3B demonstrates a stack with an error code
	; we don't expect PIC IRQ to push an error code (hey, it's not an exception!)
	mov rcx, [rsp+8]
	mov [task_save_rip], rcx
	mov rcx, [rsp+24] 
	mov [task_save_rflags], rcx
	or rcx, 0x200
	mov rcx, [rsp+32]
	mov [task_save_rsp], rcx
	pop rcx
	call task_save
	call task_switch
	call lapic_eoi_send
	call task_load
	push rcx
	mov rcx, [task_load_rip]
	mov [rsp+8], rcx
	mov rcx, [task_load_rflags]
	mov [rsp+24], rcx
	mov rcx, [task_load_rsp]
	mov [rsp+32], rcx
	pop rcx
	iretq