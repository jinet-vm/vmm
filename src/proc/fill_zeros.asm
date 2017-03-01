public _fill_zeros
; edi -> dst, ecx -> n
_fill_zeros:
	mov al, 0
	rep stosb
	ret

include 'fill_zeros.inc'

public fill_zeros
fill_zeros:
	push ebp
	mov ebp, esp
		Fill_zeros [ebp+8], [ebp+12]
	pop ebp
	ret