#include <jinet/module.h>
#include <jinet/regs.h>
#include <jinet/gdt.h>
#include <stdint.h>
// todo: abandon the hardcoded stuff
void ap_start();

void ap_init()
{
	uint64_t* cr3forap = 0x4000;
	*cr3forap = cr3_get();
	uint64_t* ap_start_ap = 0x4008;
	*ap_start_ap = &ap_start;
	uint64_t* gdtp_ap = 0x4010;
	*gdtp_ap = getGDTP();
}

void ap_start()
{
	mprint("printed from AP core");
	virt_init();
	virt_setup_vm();
	for(;;);
}