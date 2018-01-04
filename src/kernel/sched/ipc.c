#include <jinet/ipc.h>
#include <jinet/task.h>
#include <jinet/circbuf.h>
#include <jinet/module.h>

// inspiration: http://wiki.osdev.org/Message_Passing_Tutorial

static struct circbuf msgs[MAX_TASKS];

// todo: inline led to bug
#define disable_sched() { asm("cli"); }
#define enable_sched() { asm("sti"); }

void ipc_init()
{
	for(int i = 0; i<MAX_TASKS; i++)
		msgs[i] = circbuf_init(MAX_MSG_BUF);
}

// TODO: this might heavily depend on one-core configuration, fix?

void async_send(int dst, uint64_t body)
{
	// forming msg
	struct msg *M = malloc(sizeof(struct msg));
	M->src = curTask->pid;
	M->dst = dst;
	M->body = body;
	disable_sched();
	while(circbuf_count(&msgs[dst]) == msgs[dst].capacity)
	{
		enable_sched();
		noirq_sched(); // waiting for task_dst to clean things up
		disable_sched();
	}
	circbuf_push(&msgs[dst], M);
	enable_sched();
}

struct msg async_recv()
{
	struct msg out;
	disable_sched();
	int pid = curTask->pid;
	while(circbuf_empty(&msgs[pid]))
	{
		enable_sched();
		noirq_sched(); // waiting for ANYONE to clean things up
		disable_sched();
	}
	// ho-ho, someone wrote me [girl jumping on bed meme]
	//mprint("%d!", circbuf_count(&msgs[pid]));
	struct msg *M = (struct msg*)circbuf_pop(&msgs[pid]);
	out = *M;
	free(M);
	enable_sched();
	return out;
}

struct msg sync_send(int dst, uint64_t body)
{
	async_send(body, dst);
	return async_recv();
}