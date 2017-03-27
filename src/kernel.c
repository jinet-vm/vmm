/**
 * @file kernel.c
 * @brief Main kernel file.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

//#include <kernel/printf.h>

/*
#include <kernel/tty.h>
#include <kernel/debug.h>
#include <kernel/stack.h>
#include <kernel/paging.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>
#include <kernel/vga.h>
#include <kernel/memtab.h>
#include <kernel/ints.h>
#include <kernel/keyboard.h>
#include <kernel/consts.h>
#include <kernel/printf.h> */
//#include <msr.h>

#define p_entry(addr, f) (addr << 12) | f

/*
extern void sys_enter();
extern void sys_write(char* src);

void* PD_a;

extern void msr_get(uint32_t num, uint32_t* low, uint32_t *high);
extern void msr_set(uint32_t num, uint32_t low, uint32_t high);
*/

// Happy the man, and happy he alone
// He who can call today his own,
// He who, secure within, can say:
// Tomorrow do thy worst, for I have lived today

// \xC9 - ╔ \xCB - ╦ \xCD - ═ \xBB - ╗
//
// \xBA - ║ \xCC - ╠ \xCE - ╬ \xB9 - ╣
//
// \xC8 = ╚ \xBC - ╝ \xCA - ╩

void kernel_start()
{
	vga_init();
	tty_init();
	//printf("demos");
	tty_puts("64 bit c");

	for(;;);
	/*
	init_PD();
	map_page(0xB8000,0xB8000,pg_P | pg_U);
	double D = 0.042;
	vga_init();
	tty_init();
	printf("demos kernel loaded at 0x%x\n(phys = 0x%x)\n",KERNEL_VMA_ADDR,get_paddr(KERNEL_VMA_ADDR));
	idt_init();
	ints_install();
	idt_flush();
	mem_setup();
	// first pages mappinge
	map_page(0,0,pg_P | pg_U);
	// let's see BDA - 0x403 contains EBDA address >> 4
	uint16_t* ebdaba_point = 0x40e; // EBDA base address
	uint32_t ebda_point = (int)(*ebdaba_point) << 4;
	char rsdp_sig[8] = "RSD PTR "; // how does this work?! http://stackoverflow.com/questions/15140708/if-ios-is-a-32-bit-os-how-can-we-be-using-uint64-t-and-int64-t
	for(uint32_t i = ebda_point; i <= 0x100000; i+=0x1000)
		map_page(i, i, pg_P | pg_U);
	char* i;
	for(i = ebda_point; i < 0x10000; i++)
	{
		tty_putc("\n");
		//printf("add = %x\n", i);
		int j;
		for(j = 0; j<8; j++)
			if(*(i+j) != rsdp_sig[j])
				break;
		if(j != 8) continue;
		break;
	}
	printf("hello idiots: %x\n",i);
	//printf("EBDA base address: 0x%x\n", rsdp_sig >> 32);
	irq_install_handler(1, keyboard_handler);
	ints_sti();*/
}