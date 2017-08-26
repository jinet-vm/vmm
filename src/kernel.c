#include <stdint.h>
#include <stddef.h>
#include <jinet/debug.h>
#include <jinet/ints.h>
#include <jinet/idt.h>
#include <jinet/printf.h>
#include <jinet/keyboard.h>
#include <jinet/acpi.h>
#include <jinet/madt.h>
#include <jinet/heap.h>
#include <jinet/ipi.h>
#include <jinet/consts.h>
#include <jinet/ioapic.h>
#include <jinet/vmx.h>
#include <jinet/gdt.h>
#include <jinet/font.h>
#include <jinet/term.h>
#include <jinet/module.h>
#include <jinet/pci.h>
#include <jinet/mcfg.h>
#include <jinet/bootstruct.h>
#include <jinet/multiboot2.h>
#define p_entry(addr, f) (addr << 12) | f

#define title_lines 6

extern ints_sti(void);

void putc(void* none, char c)
{
	tty_putc(c);
}

void kernel_start();

extern void rt0();

struct bootstruct  __attribute__((section(".boot"))) bs =
{
	.lm_magic = BTSTR_LM_MAGIC,
	.lm_load_addr = KERNEL_VMA_ADDR,
	.lm_entry_addr = &rt0,
	.lm_mmap_addr = 0x100000000
};

struct term_dev vga =
{
	.name = "VBE_TERM",
	.type = TERM_VGA,
	.addr = 0x7c00,
	.init = term_vga_init,
	.putc = term_vga_putc
};

struct term_dev com_port =
{
	.name = "COM_PORT",
	.type = TERM_SERIAL_IO,
	.addr = 0x3F8,
	.init = term_serial_io_init,
	.putc = term_serial_io_putc
};

struct term_dev dell_serial =
{
	.name = "E5470_SERIAL",
	.type = TERM_SERIAL_MMIO,
	.addr = 0,
	.init = term_serial_mmio_init,
	.putc = term_serial_mmio_putc
};

MODULE("KERNEL");

void kernel_start()
{
	// VGA
	term_init();
	term_add(com_port);

	idt_init();
	isr_install();
	irq_install();
	idt_flush();
	mprint("IDT flushed");

	struct multiboot_mmap_entry* mmap = bs.lm_mmap_addr;
	for(int i = 0; i < bs.tr_mmap_len; i++)
	{
		uint64_t start = mmap[i].addr;
		uint64_t end = mmap[i].addr + mmap[i].len;
		mprint("mmap 0x%x%x - 0x%x%x", start >> 32, start, end >> 32, end);
	}

	for(;;);


	//term_add(vga);
	acpi_add_driver("APIC", madt_probe);
	cpci_init();
	acpi_add_driver("MCFG", mcfg_probe);
	acpi_probe();
	pcie_init();
	pci_probe();
	uint64_t num = mcfg_seg_group_count();
	if(num != MCFG_INVALID)
		mprint("mcfg number: %x", num);
	term_add(dell_serial);
	initGDTR();
	gdt_set_code(1);
	gdt_set_data(2);
	gdt_set_tss(3,104,0xffff800000000000); // host tss - 0x18
	gdt_set_tss(5,104,0xffff800000000100); // vm0 tss - 0x20
	gdt_set_tss(7,104,0xffff800000000200); // vm1 tss - 0x28
	gdt_flush();
	mprint("GDT flushed");
	tr_set(SEG(3));
	mprint("TSS set");
	//volatile int s = 1/0;
	//tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	mprint("IDT initialized.");
	//tty_setcolor(VC_DEFAULT);
	// heap not needed yet
	// // MADT
	//detect_rsdt();
	//print_sdts();
	//uint32_t madtb = detect_madt();
	lapic_setup(); // TODO: apic 32bit bochs error
	pic_enable();
	pic_disable();
	ioapic_setup();
	pit_init();
	for(uint8_t i = 0; i<=23; i++)
		ioapic_set_gate(i,32+i,0,0,0,0,0,0); // just to be on a safe side
	irq_install_handler(1, keyboard_handler);
	irq_install_handler(4, serial_handler);
	asm("xchg %bx, %bx");
	//pci_probe();
	//mprint("VIRT INIT");
	//virt_init();
	for(;;);
}
