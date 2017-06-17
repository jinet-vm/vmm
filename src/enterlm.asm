format ELF64

use64
section '.text' executable

macro mbp ; todo
{
	xchg bx, bx
}

VMX_ECX = 100000b
PAGE_PRESENT = 01b
PAGE_WRITE = 10b
PAGE_SIZE = 10000000b

include 'inc/consts.inc'

extrn kernel_start

public LongMode

LongMode: 
	
	mov ax, 0x0010
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; mov edi, 0xB8000
	; mov rcx, 500						; Since we are clearing uint64_t over here, we put the count as Count/4.
	; mov rax, 0x1F201F201F201F20			; Set the value to set the screen to: Blue background, white foreground, blank spaces.
	; rep stosq							; Clear the entire screen. 
	; ; Display "Hello World!"
	; mov rdi, 0x00b8000              
 
	; mov rax, 0x1F6C1F6C1F651F48    
	; mov [rdi],rax
 
	; mov rax, 0x1F6F1F571F201F6F
	; mov [rdi + 8], rax
 
	; mov rax, 0x1F211F641F6C1F72
	; mov [rdi + 16], rax

move_kernel:
	
	mov rsi, KERNEL_LOAD_ADDR
	mov rdi, KERNEL_PHYS_ADDR
	mov rcx, KERNEL_SIZE
	shr rcx, 3
	rep movsq

kernel_paging_setup:
	
	; PML4 kernel entry
	
	mov rdi, HHKERN_VMA_ADDR
	;mov rdi, 0xffff800000000000 ; todo: right mask?
	shr rdi, 39
	and rdi, 1FFh ; trash after 47th bit
	shl rdi, 3 ; <=> r8*=8
	add rdi, PAGING_PHYS_ADDR+PML4_OFF
	mov rax, PAGING_PHYS_ADDR+PDP_KERNEL_OFF
	or rax, PAGE_PRESENT
	stosq
	; PDP kernel entry
	mov rdi, PAGING_PHYS_ADDR+PDP_KERNEL_OFF
	mov rax, PAGING_PHYS_ADDR+PD_KERNEL_OFF
	or rax, PAGE_PRESENT
	stosq
	; PD kernel entries
	mov rdi, PAGING_PHYS_ADDR+PD_KERNEL_OFF
	mov rax, HHKERN_PHYS_ADDR
	;mov rax, 0x400000
	or rax, PAGE_SIZE or PAGE_PRESENT
	stosq
	mov rax, cr3
	mov cr3, rax

	; identity page [0, 0xffffffff] just for the sake of paging


	; stosq
	
	; ; now we'll map the kernel
	; mov rax, PAGING_PHYS_ADDR+PDP_KERNEL_OFF or PAGE_PRESENT
	; stosq
	; mov rdi, PAGING_PHYS_ADDR+PDP_KERNEL_OFF
	; mov rax, PAGING_PHYS_ADDR+PD_KERNEL_OFF or PAGE_PRESENT
	; stosq
	; ; not finished yet - 1 GiB for kernel (yet)
	; mov rcx, 512
	; mov rdi, PAGING_PHYS_ADDR+PD_KERNEL_OFF
	; mov rax, PAGE_PRESENT or PAGE_SIZE
	; .lp:
	; 	stosq
	; 	; add rax, 0x200000
	; loop .lp
	; stosq
	
	mov rax, kernel_start
	mov rsp, KERNEL_STACK_VMA_ADDR
	; jmp $
	; push rax
	jmp rax
	; jmp kernel_start