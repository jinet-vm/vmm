macro Hex_f n, dst
{
	push eax, edi
	mov eax, n
	mov edi, dst
	pushad
		call _hex_f
	popad
	popr eax, edi
}