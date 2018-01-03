#include <jinet/task.h>
#include <jinet/heap.h>
#include <jinet/module.h>
#include <jinet/io.h>
#include <jinet/printf.h>
#include <jinet/vga.h>
#include <jinet/ipc.h>

MODULE("TASK");

extern void tasking_enter();

struct task* curTask;
int proc_n = 0;
int proc_i = 0;

struct task* T[MAX_TASKS];

static int s1, s2;
static int p1, p2;
int t2i = 0;

char* task1_str;
char* task2_str;

static int task0() // terminal task
{
	int x = 0, y = 0, color = 0, t = 0;
	while(1)
	{
		mprint("running %d", curTask->pid);
		struct msg M = async_recv();
		mprint("task%d sent %d", M.src, M.body);
		//noirq_sched();
	}
}

static int task1()
{
	mprint("running %d", curTask->pid);
	int i = 0;
	while(1)
	{
		int fib_0 = 0, fib_1 = 1, t;
		while(1)
		{
			t = fib_1 + fib_0;
			fib_0 = fib_1;
			fib_1 = t;
			async_send(0, fib_1);
		}

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
	mprint("running %d", curTask->pid);
	int i = 0;
	while(1)
	{
		int i = 0;
		while(1)
			async_send(0, i++);
	}
}

int sched_init()
{
	mprint("sched_init! start");
	ipc_init();
	sched_addt(task_create("task0", task0));
	sched_addt(task_create("task1", task1));
	sched_addt(task_create("task2", task2));
	mprint("tasking_enter start");
	curTask = T[0];
	mprint("curTask at 0x%llx", curTask);
	// TODO: become a better person
	T[0]->next = T[1];
	T[1]->next = T[2];
	T[2]->next = T[0]; // I can go on forever now
	pit_init();
	tasking_enter();
	// waiting for irq_sched
}

int sched_addt(struct task* t)
{
	if(proc_i >= MAX_TASKS) // not enough space in T
		return 1;
	t->pid = proc_n;
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
	//T->pid = proc_n
	T->rip = thread;
	//T->rflags = 0x200;
	T->name = name;
	// stack setup
	T->rsp = malloc(0x2000) + 0x2000; // TODO: it's horrible; do it with physmm_alloc in mind
	return T;
}