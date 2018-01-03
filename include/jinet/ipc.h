/**
 * \brief Very basic IPC system.
 */

#ifndef IPC_H
#define IPC_H

#include <stdint.h>

#define MAX_MSG_BUF 30

struct msg
{
	int src;
	int dst;
	uint64_t body;
};

void async_send(uint64_t body, int dst);
struct msg async_recv();
struct msg sync_send(uint64_t body, int dst);
void ipc_init();
// struct msg sync_recv(); - i can't really work out what on earth is that

#endif