format binary
org 0x7000

start: ; running in comp. mode
	mov ax, 42
	mov si, str1
	mov cx, 15
	.lp:
	mov ax, 42
	mov bx, cx
	vmcall
	loop .lp
	jmp $

str1: db "Hello! I am VM1"

finish:
times 0xFC-finish+start db 0
dd 0xdeadbeef