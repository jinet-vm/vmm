#include <jinet/task.h>
#include <jinet/heap.h>
#include <jinet/module.h>
#include <jinet/io.h>
#include <jinet/printf.h>;

MODULE("TASK");

#define MAX_TASK 10

extern void tasking_enter();

struct task* curTask;
int proc_n = 0;
int proc_i = 0;

struct task* T[MAX_TASK];

static int s1, s2;
static int p1, p2;
int t2i = 0;

static int task0()
{
	while(1)
	{
		COM_putc('0');
		COM_putc(' ');
		// if(p1)
		// {
		// 	mprint("task1: %d", s1);
		// 	s1 = 0;
		// 	p1 = 0;
		// }
		// if(p2)
		// {
		// 	mprint("task2: %d", s2);
		// 	s2 = 0;
		// 	p2 = 0;
		// }
	}
}

static int task1()
{
	int i = 0;
	while(1)
	{
		//printf("1");
		// if(!p1)
		// {
		// 	i++;
		// 	s1 = i;
		// 	p1 = 1;
		// }
	}
}


static int task2()
{
	
	while(1)
	{
		t2i++;
		//printf("2");
		// mprint("2");
		// if(!p2)
		// {
		// 	i++;
		// 	s2 = i;
		// 	p2 = 1;
		// }
	}
}

int sched_init()
{
	mprint("sched_init! start");
	sched_addt(task_create("task0", task0));
	sched_addt(task_create("task1", task1));
	sched_addt(task_create("task2", task2));
	mprint("tasking_enter start");
	curTask = T[0];
	mprint("curTask at 0x%llx", curTask);
	asm("xchg %bx, %bx");
	T[0]->next = T[0];
	T[1]->next = T[2];
	T[2]->next = T[0]; // I can go on forever now
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