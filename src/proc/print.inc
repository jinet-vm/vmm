macro Print src, y, color
{
	push esi, ebx, edx
	mov esi, src
	mov ebx, y
	mov edx, color
	call _print
	popr esi, ebx, edx
}