.PHONY: all prepare clean kernel bootloader configure

SHELL=bash
CC=gcc
CFLAGS = -m64 -o0 -Iinclude -ffreestanding -nostdlib -lgcc -w -mcmodel=large
KERNEL_START = 0x0
AS=fasm

consts_h := include/kernel/consts.h
consts_ld := consts.ld
consts_inc := inc/consts.inc

prepare:
	mkdir -p obj
	mkdir -p bin

configure: config.json
	# consts.h generation
	touch $(consts_h)
	cat /dev/null > $(consts_h)
	printf "#ifndef CONSTS_H\n#define CONSTS_H\n\n" >> $(consts_h)
	./configure.py --lang c >> $(consts_h)
	printf "\n#endif" >> $(consts_h)
	# consts.ld generation
	touch $(consts_ld)
	cat /dev/null > $(consts_ld)
	./configure.py --lang ld >> $(consts_ld)
	# consts.inc generation
	touch $(consts_inc)
	cat /dev/null > $(consts_inc)
	./configure.py --lang fasm >> $(consts_inc)

$(consts_ld): config.json configure
$(consts_h): config.json configure
$(consts_inc): config.json configure

obj/boot.o: src/boot.asm
	mkdir -p obj/boot
	$(AS) src/boot.asm obj/boot/boot.o

obj/procedures.o: src/procedures.asm
	$(AS) src/procedures.asm obj/procedures.o

obj/enable_paging.o: src/memory/enable_paging.asm
	$(AS) src/memory/enable_paging.asm obj/enable_paging.o

obj/msr.o: src/misc/msr.asm
	$(AS) src/misc/msr.asm obj/msr.o

obj/sys_enter.o: src/usermode/sys_enter.asm
	$(AS) src/usermode/sys_enter.asm obj/sys_enter.o

# obj/main.o: src/kernel.c obj/boot.o obj/tty.o obj/stack.o obj/enable_paging.o obj/gdt.o obj/tss.o obj/vga.o obj/memtab.o obj/paging.o obj/msr.o obj/ints.o obj/keyboard.o obj/printf.o
#	$(CC) $(CFLAGS) -c src/kernel.c -o obj/main.o -g
obj/main.o: src/kernel.c obj/vga.o obj/tty.o obj/ints.o obj/idt.o obj/keyboard.o obj/acpi.o # obj/printf.o
	$(CC) $(CFLAGS) -c src/kernel.c -o obj/main.o -g

obj/memory.o: src/memory/memory.c include/kernel/memory.h
	$(CC) $(CFLAGS) -c src/memory/memory.c -o obj/memory.o

obj/memtab.o: src/memory/memtab.c obj/memory.o obj/tss.o obj/gdt.o
	$(CC) $(CFLAGS) -c src/memory/memtab.c -o obj/memtab.o

obj/tty.o: src/vga/tty.c obj/vga.o
	$(CC) $(CFLAGS) -c src/vga/tty.c -o obj/tty.o	

obj/vga.o: src/vga/vga.c obj/io.o obj/memory.o
	$(CC) $(CFLAGS) -c src/vga/vga.c -o obj/vga.o	

obj/io.o: src/misc/io.c
	$(CC) $(CFLAGS) -c src/misc/io.c -o obj/io.o

obj/stack.o: src/memory/stack.c
	$(CC) $(CFLAGS) -c src/memory/stack.c -o obj/stack.o

obj/gdt.o: src/memory/gdt.c
	$(CC) $(CFLAGS) -c src/memory/gdt.c -o obj/gdt.o

obj/tss.o: obj/desc.o
	$(CC) $(CFLAGS) -c src/usermode_kernel/tss.c -o obj/tss.o

obj/paging.o: src/memory/paging.c
	$(CC) $(CFLAGS) -c src/memory/paging.c -o obj/paging.o

obj/desc.o: src/memory/desc.asm
	$(AS) src/memory/desc.asm obj/desc.o

obj/isrq.o: src/idt/isrq.asm
	$(AS) src/idt/isrq.asm obj/isrq.o

obj/ints.o: src/idt/ints.c obj/idt.o obj/isrq.o obj/isr.o obj/irq.o
	$(CC) $(CFLAGS) -c src/idt/ints.c -o obj/ints.o

obj/idt.o: src/idt/idt.c
	$(CC) $(CFLAGS) -c src/idt/idt.c -o obj/idt.o

obj/irq.o: src/idt/irq.c obj/idt.o obj/io.o
	$(CC) $(CFLAGS) -c src/idt/irq.c -o obj/irq.o

obj/isr.o: src/idt/isr.c obj/idt.o obj/printf.o
	$(CC) $(CFLAGS) -c src/idt/isr.c -o obj/isr.o

obj/keyboard.o: src/misc/keyboard.c obj/tty.o obj/io.o
	$(CC) $(CFLAGS) -c src/misc/keyboard.c -o obj/keyboard.o

obj/printf.o: src/vga/printf.c
	$(CC) $(CFLAGS) -c src/vga/printf.c -o obj/printf.o

obj/acpi.o: src/acpi/acpi.c
	$(CC) $(CFLAGS) -c src/acpi/acpi.c -o obj/acpi.o

enterlm.img: src/enterlm.asm kernel
	mkdir -p obj/enterlm/
	$(AS) src/enterlm.asm obj/enterlm/enterlm.o
	ld -T $(consts_ld) -T enterlm.ld obj/enterlm/*.o -M -melf_x86_64 --defsym kernel_start=$(shell cat kernel_start.sym) > enterlm.map
	cat enterlm.map | grep LongMode | tr ' ' '\n' | grep 0x > longmode.sym

kernel: kernel.ld obj/main.o obj/boot.o $(consts_h)
	ld -T $(consts_ld) -T kernel.ld obj/*.o -M -melf_x86_64 > kernel.map
	cat kernel.map | grep kernel_start | tr ' ' '\n' | grep 0x > kernel_start.sym

bootloader: obj/boot.o enterlm.img $(consts_ld)
	ld -T $(consts_ld) -T boot.ld obj/boot/boot.o -melf_i386 --defsym LongMode=$(shell cat longmode.sym) # boot.img

all: prepare kernel bootloader enterlm.img
	cp boot.img final.img
	cat enterlm.img >> final.img
	cat kernel.img >> final.img
	# cat bin/rmint.bin >> final.img
	wc -c final.img

clean:
	rm obj/* -rf
	rm -f *.img
	rm -f *.map
	rm -f *.sym
	rm -f kernel_start