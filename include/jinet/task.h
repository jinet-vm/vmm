#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <jinet/term.h>

#define PROCESS_STATE_ALIVE 1
#define PROCESS_STATE_DEAD 0
/**
 * @brief      Contains data about thread.
 */
struct task
{
	uint64_t rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, rip;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
	uint64_t rflags;
	char* name;
	uint64_t pid;
	struct termbuf *tb;
	//uint64_t cr3; - nah, just threads
	//uint64_t state;
	struct task *next;
} __attribute__((packed));

int sched_init();
int sched_addt(struct task* t);
struct task* task_create(char* name, int (*thread)(void));
extern struct task* curTask;
extern void noirq_sched();
#endif