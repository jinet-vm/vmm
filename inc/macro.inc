; Debugging

DEBUG = 0

macro mbp
{
	if DEBUG=1
		xchg bx, bx
	end if
}

; Stack ops

macro push [arg] { push arg }

macro popr [arg] { reverse pop arg }
; now we can:
; 1. push a, b, c
; 2 .pop a, b,c
; 3. PROFIT

macro pop [arg] { forward pop arg } 


; Procedures (inspired by http://asmworld.ru/uchebnyj-kurs/029-makrosy-proc-i-endp/)
; cdecl (!)
; no proc macro - can't use local labels there
; TODO: do a yet another proc macro?

macro ccall proc, [arg] ; result expected - don't touch eax
{
	common	argS = 0
			; push all except eax
			pushad
			pushf
	reverse push arg
			argS = argS + 4
	common 	call proc
	common 	add esp, argS
			popf
			popad
			; popad
}

macro ccallr proc, [arg]
{
	common	argS = 0
			; push all except eax
			push ecx, edx, ebx, ebp, esi, edi
			pushf
	reverse push arg
			argS = argS + 4
	common 	call proc
	common 	add esp, argS
			popf
			popr ecx, edx, ebx, ebp, esi, edi
			; popad
}