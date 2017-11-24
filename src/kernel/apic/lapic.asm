format elf64

section '.text' executable
use64

; get it from apic.c
extrn lapic

; public lapic_reg_read
; public lapic_reg_write

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

; lapic_reg_write: ; rdi = int n, rsi = uint32_t val
; 	push rsi
; 	push rdi
; 	push rcx
; 	mov rcx, [lapic]
; 	shl rdi, 4
; 	add rcx, rdi
; 	mov [rcx], rsi
; 	pop rcx
; 	pop rdi
; 	pop rsi
; 	ret