format binary

include '../inc/consts.inc'

use64

;xchg bx, bx
mov rsp, KERNEL_STACK_VMA_ADDR-0x1000 ; todo: bootstruct way?
mov rax, [rax]
jmp rax