format ELF

section '.data' executable
use16

; the same is done in boot.asm for pre-init loading

section '.text32' executable
use32

public setGDTR
setGDTR:
	push ebp
	mov ebp, esp
	push esi
	mov esi, [ebp+8]
	lgdt fword [esi]
	pop esi
	pop ebp
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