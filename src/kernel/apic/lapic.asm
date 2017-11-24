format elf64

section '.text' executable
use64

; get it from apic.c
extrn lapic

; public lapic_reg_read
public ls_eoi

; lapic_reg_read: ; rdi = int n
; 	push rsi
; 	push rdi
; 	mov rsi, [lapic]
; 	shl rdi, 4
; 	add rsi, rdi
; 	mov rax, [rsi]
; 	pop rdi
; 	pop rsi
; 	ret

ls_eoi: ; rdi = int n, rsi = uint32_t val
	push rsi
	mov rsi, 0xfee000B0
	mov dword [rsi], 0
	pop rsi
	ret