format elf64

section '.data' executable
use16

; the same is done in boot.asm for pre-init loading

section '.text' executable
use64

public setGDTR
setGDTR:
	; push ebp
	; mov ebp, esp
	; push esi
	; mov esi, [ebp+8]
	; lgdt fword [esi]
	; pop esi
	; pop ebp
	ret

public setTR
setTR: ;(uint16_t segment)
	; push ebp
	; mov ebp, esp
	; push eax
	; mov eax, [ebp+8]
	; ltr ax
	; pop eax
	; pop ebp
	ret

; todo: use inline asm