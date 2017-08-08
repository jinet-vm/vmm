format elf

section '.multiboot' align 8

; 0	u32	magic	required 
; 4	u32	flags	required 
; 8	u32	checksum	required 
; 12	u32	header_addr	if flags[16] is set 
; 16	u32	load_addr	if flags[16] is set 
; 20	u32	load_end_addr	if flags[16] is set 
; 24	u32	bss_end_addr	if flags[16] is set 
; 28	u32	entry_addr	if flags[16] is set 
; 32	u32	mode_type	if flags[2] is set 
; 36	u32	width	if flags[2] is set 
; 40	u32	height	if flags[2] is set 
; 44	u32	depth	if flags[2] is set 

magic = 0x1BADB002
flags = 0
chksm = -(magic+flags)

multiboot2_header:
	.magic:			dd magic			; required
	.flags:			dd flags			; required
	.checksum:		dd chksm			; required
	.header_addr:	dd 0				; if flags[16] is set
	.load_addr:		dd 0				; if flags[16] is set
	.load_end_addr:	dd 0				; if flags[16] is set
	.bss_end_addr:	dd 0				; if flags[16] is set
	.entry_addr:	dd 0				; if flags[16] is set
	.mode_type:		dd 0				; if flags[2] is set
	.width:			dd 0				; if flags[2] is set
	.height:		dd 0				; if flags[2] is set
	.depth:			dd 0				; if flags[2] is set