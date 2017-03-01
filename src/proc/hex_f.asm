public _hex_f
; yet another itoa
; abcdefgh - format idea (8 hex digits)
; hex_f(int n, char* s) // hex
; eax - n, edi - destination
_hex_f:
	add edi, 7
	std
	mbp
	mov ecx, 8
	.lp:
		mov edx, eax
		and eax, 0xF
		mov al, [.symbt+eax]
		stosb
		mov eax, edx
		shr eax, 4
	loop .lp
	cld
	ret
	.symbt: db '0123456789ABCDEF'

include 'hex_f.inc'

public hex_f
hex_f:
	push ebp
	mov ebp, esp
		Hex_f [ebp+8], [ebp+12]
	pop ebp
	ret