#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define PROCESS_STATE_ALIVE 1
#define PROCESS_STATE_DEAD 0
struct task
{
	// registers order MUST stay the same
	uint64_t rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, rip;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
	char* name;
	//uint64_t pid; - as for now, living in P
	//uint64_t cr3; - nah, just threads
	//uint64_t state;
	//process *next;
} __attribute__((packed));

int sched_init();
int sched_addt(struct task* t);
struct task* task_create(char* name, int (*thread)(void));
struct task* task_by_id(int id);
struct task* task_switch();

extern struct* curTask;
extern int proc_n; // TODO: better!
extern int proc_n; // TODO: better!
#endif