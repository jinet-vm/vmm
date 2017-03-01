format ELF

section '.text' executable

include 'inc/macro.inc'

; TODO: cpuid check

public msr_get ; void msr_get(uint32_t num, uint32_t* high, uint32_t *low)
msr_get:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	rdmsr
	push esi
	mov esi, [ebp+16]
	mov [esi], eax
	mov esi, [ebp+12]
	mov [esi], edx
	pop esi
	pop ebp
	ret

public msr_set; void (uint32_t num, uint32_t high, uint32_t low)
msr_set:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	mov eax, [ebp+16]
	mov edx, [ebp+12]
	wrmsr
	pop ebp
	ret