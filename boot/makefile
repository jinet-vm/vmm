.PHONY: all

clean:
	rm -f *.o
	rm -f boot.elf
	rm -f boot.img

all: boot.o enterlm.o
	ld -Tboot.ld boot.o enterlm.o -melf_i386

boot.o:
	fasm boot.asm boot.o

enterlm.o:
	gcc -I../include -std=gnu11 -m32 -o0 -ffreestanding -nostdlib -lgcc -w -fno-pic -fpermissive -fno-stack-protector -c enterlm.c -o enterlm.o
