.PHONY: all clean kernel sysr

SHELL=bash
CC=gcc
CFLAGS = -m32 -o0 -Iinclude -ffreestanding -nostdlib -lgcc -w

AS=fasm

obj/boot.o: src/boot.asm
	$(AS) src/boot.asm obj/boot.o

obj/procedures.o: src/procedures.asm
	$(AS) src/procedures.asm obj/procedures.o

obj/enable_paging.o: src/memory/enable_paging.asm
	$(AS) src/memory/enable_paging.asm obj/enable_paging.o

obj/msr.o: src/misc/msr.asm
	$(AS) src/misc/msr.asm obj/msr.o

obj/sys_enter.o: src/usermode/sys_enter.asm
	$(AS) src/usermode/sys_enter.asm obj/sys_enter.o

obj/main.o: src/kernel.c obj/boot.o obj/tty.o obj/stack.o obj/enable_paging.o obj/gdt.o obj/tss.o obj/vga.o obj/memtab.o obj/paging.o obj/msr.o obj/ints.o obj/keyboard.o
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

obj/isr.o: src/idt/isr.c obj/idt.o
	$(CC) $(CFLAGS) -c src/idt/isr.c -o obj/isr.o

obj/keyboard.o: src/misc/keyboard.c obj/tty.o obj/io.o
	$(CC) $(CFLAGS) -c src/misc/keyboard.c -o obj/keyboard.o

kernel: kernel.ld obj/main.o obj/boot.o
	cp kernel.ld obj
	cd obj
	ld -T kernel.ld -melf_i386 obj/*.o
	wc -c final.img

clean:
	rm obj/* -rf
	rm -f final.img