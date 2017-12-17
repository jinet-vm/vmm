.PHONY: all

clean:
	rm -f boot/*.o
	rm -f boot/boot.elf
	rm -f boot/lm.bin

all: boot/boot.o boot/enterlm.o
	ld -Tboot.ld boot/boot.o boot/enterlm.o -melf_i386

boot/lm.bin: src/grub_if/lm.asm
	fasm src/grub_if/lm.asm boot/lm.bin

boot/boot.o: src/grub_if/boot.asm boot/lm.bin
	fasm src/grub_if/boot.asm boot/boot.o

boot/enterlm.o: src/grub_if/enterlm.c
	gcc -Iinclude -std=gnu11 -m32 -o0 -ffreestanding -nostdlib -lgcc -w -fno-pic -fpermissive -fno-stack-protector -c src/grub_if/enterlm.c -o boot/enterlm.o
