// DBGP and DBG2 - the Debug Port(s) tables

#include <jinet/dbgp.h>
#include <jinet/acpi.h>
#include <jinet/module.h>
#include <stdint.h>

MODULE("ACPI_DBGP");

#define INTERFACE_UART16550 0
#define INTERFACE_UART16550_DBGP_CMPT 1

static int ok = 1;

struct dbgp
{
	struct sdt_header h;
	uint8_t interface;
	uint8_t _r0, _r1, _r2;
	struct gras base_addr;
} __attribute__ ((packed));

static struct dbgp* DBGP;

int dbgp_probe(void* table)
{
	DBGP = table;
	mprint("base addr: %llx", DBGP->base_addr.addr);
	return 0;
}

uint64_t dbgp_base_addr()
{
	return DBGP->base_addr.addr;
}

int dbgp_ok()
{
	ok = 0;
	return ok;
}