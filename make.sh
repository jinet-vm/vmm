#!/bin/bash
echo ">>> init"
mkdir -p obj
rm obj/* -rf
CYAN='\033[0;36m'
NC='\033[0m' # No Color
printf "${CYAN}>> BOOT & INIT <<${NC}\n"
echo ">>> fasm"
fasm src/boot.asm obj/boot.o
fasm src/procedures.asm obj/procedures.o
fasm src/paging.asm obj/paging.o
fasm src/msr.asm obj/msr.o
fasm src/sys_enter.asm obj/sys_enter.o
echo ">>> gcc"
gcc -m32 -o0 -c src/kernel.c -o obj/main.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/memory.c -o obj/memory.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/memtab.c -o obj/memtab.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/tty.c -o obj/tty.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/stack.c -o obj/stack.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/paging.c -o obj/paging.c.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/gdt.c -o obj/gdt.o -Iinclude -ffreestanding -nostdlib -lgcc -w
gcc -m32 -o0 -c src/tss.c -o obj/tss.o -Iinclude -ffreestanding -nostdlib -lgcc -w
echo ">>> linking boot & init"
$(cp linker.ld obj; cd obj; ld -T linker.ld -melf_i386 *.o)
printf "${CYAN}>> SYS_ROUTINE <<${NC}\n"
mkdir obj/sysr
fasm src/sys_routine.asm obj/sysr/sys_routine.o # flat binary _now_
$(cp sysr.ld obj; cd obj; ld -T sysr.ld -melf_i386 sysr/sys_routine.o)
cat obj/sysr.bin >> obj/final.img
if [ -f obj/final.img ]; then
	mv obj/final.img .
fi
echo ">>> final.img size"
wc -c final.img
