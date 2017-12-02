#include <jinet/task.h>
#include <jinet/heap.h>
#include <jinet/module.h>
#include <jinet/io.h>
#include <jinet/printf.h>
#include <jinet/vga.h>

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

char* task1_str;
char* task2_str;

static int task0() // terminal task
{
	int x = 0, y = 0, color = 0, t = 0;
	while(1)
	{
		if(task1_str)
			{
				mprint("t1: %s", task1_str);
				task1_str = 0;
			}
		if(task2_str)
			{
				mprint("t2: %s", task2_str);
				task2_str = 0;
			}
	}
}

static int task1()
{
	int i = 0;
	while(1)
	{
		i++;
		if(!task1_str)
			if(i % 2 == 0)
				task1_str = "dddd!";
			else
				task1_str = "bbbb!";
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
	int i = 0;
	while(1)
	{
		i++;
		if(!task2_str)
			if(i % 2 == 1)
				task2_str = "oooo!!";
			else
				task2_str = "aaaa!";
		//printf("2   ");
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
	T[0]->next = T[1];
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