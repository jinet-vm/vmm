format elf64

section '.text' executable

use64

; macro SC addr
; {
; 	dd addr
; }

SYSCALL_NUMBER = 1

sys_routine:
; 	cmp eax, SYSCALL_NUMBER
; 	jg .end
; 	call dword SC_print
; 	.end:
; 	sysexit

; syscalls: 
; 	SC SC_print

; SC_print: ; esi - char* src
; 	pushad
; 		mov edi, 0xb8000
; 		mov ah, 0x05
; 		.loop:
; 		lodsb
; 		test al, al
; 		jz .exit
; 		stosw
; 		jmp .loop
; 		.exit:
; 	popad
; 	ret