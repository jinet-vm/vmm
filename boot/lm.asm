format binary
use64

xchg bx, bx
mov rax, [rax]
jmp rax
