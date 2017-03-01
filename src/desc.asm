format ELF

section '.text32' executable
use32

public getGDTR
getGDTR:
	mov eax, GDTR
	ret

public setGDTR
setGDTR:
	lgdt fword [GDTR]
	ret

public setTR
setTR: ;(uint16_t segment)
	push ebp
	mov ebp, esp
	push eax
	mov eax, [ebp+8]
	ltr ax
	pop eax
	pop ebp
	ret