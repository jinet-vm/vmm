/**
 * @file kernel.c
 * @brief Main kernel file.
 */

#include <stdint.h>
//#include <stdlib.h>
#include <stddef.h>
#include <kernel/debug.h>
#include <kernel/ints.h>
#include <kernel/idt.h>
#include <kernel/printf.h>
#include <kernel/keyboard.h>
#include <kernel/acpi.h>
#include <kernel/madt.h>
#include <kernel/heap.h>
#include <kernel/ipi.h>
#include <kernel/consts.h>
#include <kernel/ioapic.h>
#include <kernel/vmx.h>
#include <kernel/gdt.h>
#include <kernel/font.h>
#define p_entry(addr, f) (addr << 12) | f

#define title_lines 6

char* title[title_lines] = 
{
	"                         \xDC\xDC\xDC\xDC\xDC \xDC    \xDC   \xDC\xDB\xDB\xDB\xDC     \xDC\xDC\xDC\xDC\xDF v0.0.1 (WIP)\n",
	"                       \xDC\xDF  \xDB   \xDB\xDB     \xDB  \xDB\xDF   \xDF \xDF\xDF\xDF \xDB    \n",
	"                           \xDB   \xDB\xDB \xDB\xDB   \xDB \xDB\xDB\xDC\xDC       \xDB    \n",
	"                        \xDC \xDB    \xDE\xDB \xDB \xDB  \xDB \xDB\xDC   \xDC\xDF   \xDB     \n",
	"                         \xDF      \xDE \xDB  \xDB \xDB \xDF\xDB\xDB\xDB\xDF    \xDF      \n",
	"                                  \xDB   \xDB\xDB\n\n"
};

extern ints_sti(void);

void putc(void* none, char c)
{
	tty_putc(c);
}

void kernel_start()
{
	// VGA
	vga_init();
	tty_init();
	init_printf(NULL,putc);
	// tty_putc('\n');
	// tty_putc('\n');
	// struct vga_pixel white;
	// white.r=white.g=white.b=0xff;
	// vga_putc(0xdb,white,0,0);
	// for(;;);
	//  // vga_put_pixel(0,0,white);
	//  // vga_put_pixel(0,1,white);
	//  // vga_put_pixel(0,2,white);
	// unsigned char c = 0xDB;
	// for(int j = 0; j<16; j++)
	// {
	// 	int s = c;
	// 	s <<= 4;
	// 	s += j;
	// 	uint8_t line = the_font[s];
	// 	printf("%x and\n",line);
	// 	for(int i = 0; i<8; i++)
	// 		if((line >> (8-i)) & 1)
	// 			vga_put_pixel(0*8+i,0*16+j,white);
	// }
	// asm("xchg %bx, %bx");
	// //for(;;);
	// //vga_putc(176,white,0,0);
	// // for(int i = 0; i<16; i++)
	// // {
	// // 	for(int j=0; j<16; j++)
	// // 		if((i*16 + j) == '\n') vga_putc(' ');
	// // 		else tty_putc(0xb7);
	// // 	tty_putc('\n');
	// // }
	
	// for(int i = 0; i<16; i++)
	// 	printf("%02x ", the_font[0xdb0+i]);
	// printf("\nADDRESS: 0x%x%08x",((uint64_t)the_font+'Z'*3lu)>>32,((uint64_t)the_font+'Z'*3lu));
	// for(;;);
	tty_setcolor(vga_color(VC_LIGHT_BLUE,VC_BLACK));
	for(int i = 0; i<title_lines; i++)
		tty_puts(title[i]);
	for(;;);
	tty_setcolor(VC_DEFAULT);
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	init_printf(NULL,putc);
	printf("VGA terminal initialized.\n");
	// IDT
	idt_init();
	isr_install();
	irq_install();
	idt_flush();
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("IDT initialized.\n");
	tty_setcolor(VC_DEFAULT);
	// heap not needed yet
	// // MADT
	// detect_rsdt();
	// print_sdts();
	// uint32_t madtb = detect_madt();
	// lapic_setup(); // TODO: apic 32bit bochs error
	// tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	// printf("MADT & LAPIC initialized.\n");
	// tty_setcolor(VC_DEFAULT);
	// asm("xchg %bx, %bx");
	//printf("MADT & LAPIC initialized.\n");
	//pic_disable();
	//ioapic_setup();
	//asm("sti");
	//pit_init();
	// todo: crazy stuff here!
	// VMX
	//printf("hey!\n");
	//volatile int a = 1/0;
	initGDTR();
	gdt_set_code(1);
	gdt_set_data(2);
	gdt_set_tss(3,104,0xffff800000000000);
	gdt_flush(5);
	for(;;);
	asm("xchg %bx, %bx");
	tr_set(SEG(3));
	virt_init();
	for(;;);
}
