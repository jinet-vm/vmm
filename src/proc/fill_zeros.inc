macro Fill_zeros dst, n
{
	push edi, ecx
	mov edi, dst
	mov ecx, n
	call _fill_zeros
	popr edi, ecx
}