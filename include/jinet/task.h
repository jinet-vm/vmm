#ifndef TASK_H
#define TASK_H

#define PROCESS_STATE_ALIVE 1
#define PROCESS_STATE_DEAD 0
struct process
{
	uint64_t rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, rip;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
	char *name;
	uint64_t pid;
	uint64_t cr3;
	uint64_t state;
} __attribute__((packed));

int sched_init();
int sched_addp(struct process* p);
struct process* process_create(char* name, int (*thread)(void));

#endif