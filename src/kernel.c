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
extern vmx_check();


void kernel_start()
{
	// VGA
	vga_init();
	tty_init();
	tty_setcolor(vga_color(VC_LIGHT_BLUE,VC_BLACK));
	for(int i = 0; i<title_lines; i++)
		tty_puts(title[i]);
	tty_setcolor(VC_DEFAULT);
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("VGA terminal initialized.\n");
	// IDT
	idt_init();
	isr_install();
	irq_install();
	idt_flush();
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("IDT initialized.\n");
	tty_setcolor(VC_DEFAULT);
	// Heap
	//heap_init();
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("Heap initialized.\n");
	tty_setcolor(VC_DEFAULT);
	// MADT
	detect_rsdt();
	print_sdts();
	uint32_t madtb = detect_madt();
	// if(!madtb)
	// {
	// 	tty_setcolor(vga_color(VC_RED,VC_BLACK));
	// 	printf("ERROR: No MADT.\n");
	// 	for(;;);
	// }
	//detect_cpu_topology();
	lapic_setup(); // TODO: apic 32bit bochs error
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("MADT & LAPIC initialized.\n");
	tty_setcolor(VC_DEFAULT);
	//mbp;
	/*
	void* a = malloc(0x100);
	void* b = malloc(0x100);
	void* c = malloc(0x100);
	void* d = malloc(0x100);
	printf("b = 0x%x%x\n",(uint64_t)b >> 32, b);
	free(b);
	heap_show_blocks();*/
	// memcpy(0x7000, 0x10600, 0x1000);
	// //mbp;
	// ipi_send(0x7,DLM_INIT,DSM_PHYS,LVL_INIT,TRG_EDGE,DSH_NO,1);
	// ipi_send(0x7,DLM_SIPI,DSM_PHYS,LVL_DEF,TRG_EDGE,DSH_NO,1);
	//mbp;
	//idt_flush();
	pic_disable();
	ioapic_setup();
	pit_init();
	ioapic_set_gate(1,33,0,0,0,0,0,0);
	irq_install_handler(1, keyboard_handler);
	printf("IOAPIC setup keyboard\n");
	//cdints_sti();
	printf("STI...\n");
	memcpy(0x7000, 0x10600, 0x1000);
	//ints_sti();
	//asm volatile("cli");
	// ipi_send(0x7,DLM_INIT,DSM_PHYS,LVL_INIT,TRG_EDGE,DSH_NO,1);
	// // asm volatile("sti");
	// // pit_sleep(10);
	// // asm volatile("cli");
	// ipi_send(0x7,DLM_SIPI,DSM_PHYS,LVL_DEF,TRG_EDGE,DSH_NO,1);
	// // asm volatile("sti");
	// // pit_sleep(10);
	// // asm volatile("cli");
	// ipi_send(0x7,DLM_SIPI,DSM_PHYS,LVL_DEF,TRG_EDGE,DSH_NO,1);
	//asm volatile("sti");
	// mbp;
	// ioapic_set_gate(1,33,0,0,0,0,0,0);
	//printf("one and ");
	//pit_sleep(1000);
	//mbp;
	//pit_sleep(1000);
	//printf("two");
	//detect_cpu_topology();
	//printf("MADT: 0x%x\n",find_sdt("APIC"));
	//mbp;
	//volatile int a = 1/0;
	printf("vmx: %d",vmx_check());
	for(;;);
	/*
	init_PD();
	map_page(0xB8000,0xB8000,pg_P | pg_U);
	double D = 0.042;
	vga_init();
	tty_init();
	printf("demos kernel loaded at 0x%x\n(phys = 0x%x)\n",KERNEL_VMA_ADDR,get_paddr(KERNEL_VMA_ADDR));
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
	*/
}