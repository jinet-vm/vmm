; basic procedures for IO and other life stuff
; registers are used to store arguments (kernel mode)
; the caller clears regs

; proc - regs
; _proc - cdecl
; Proc - macro

format ELF

include 'inc/macro.inc'

section '.text' executable
Use32

include 'proc/fill_zeros.asm'
include 'proc/hex_f.asm'
include 'proc/write.asm'
include 'proc/print.asm'