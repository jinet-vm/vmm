macro Write src, x, y, color
{
	push esi, eax, ebx, edx
	mov esi, src
	mov eax, x
	mov ebx, y
	mov edx, color
	call _write
	popr esi, eax, ebx, edx
}