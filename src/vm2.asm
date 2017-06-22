format binary
org 0x7100

use64

start: ; running in comp. mode
	mov ax, 42
	mov si, str1
	mov cx, 16
	mov bx, 5
	vmcall
	jmp $

str1: db "Hello! I am VM2!"

finish:
times 0xFC-finish+start db 0
dq 0xdeadbeef