#include <jinet/task.h>
#include <jinet/heap.h>
#include <jinet/module.h>

MODULE("TASK");

#define MAX_TASK 10

extern void tasking_enter();

struct task* curTask;
int proc_n = 0;
int proc_i = 0;

struct task* T[MAX_TASK];

static int task0()
{
	while(1)
	{
	mprint("A");
		asm("sti");
	}
}

static int task1()
{
	while(1)
	{
	mprint("B");
		asm("sti");
	}
}

static int task2()
{
	while(1)
	{
	mprint("C");
		asm("sti");
	}
}

int sched_init()
{
	mprint("sched_init! start");
	sched_addt(task_create("task0", task0));
	sched_addt(task_create("task1", task1));
	curTask = T[0];
	mprint("tasking_enter start");
	mprint("curTask at 0x%llx", curTask);
	asm("xchg %bx, %bx");
	curTask->next = T[1];
	T[1]->next = task_create("task2", task2); // I can go on forever now
	(T[1]->next)->next = T[0];
	pit_init();
	tasking_enter();
	// waiting for irq_sched
}

int sched_addt(struct task* t)
{
	if(proc_i >= MAX_TASK) // not enough space in T
		return 1;
	T[proc_n++] = t;
}

struct task* task_by_id(int id)
{
	if(id >= proc_i) return 0;
}

struct task* task_create(char* name, int (*thread)(void))
{
	struct task* T = malloc(sizeof(struct task));
	memset(T,0,sizeof(struct task));
	T->rip = thread;
	//T->rflags = 0x200;
	T->name = name;
	// stack setup
	T->rsp = malloc(0x2000) + 0x2000; // TODO: it's horrible; do it with physmm_alloc in mind
	return T;
}