#ifndef STACK_H
#define STACK_H

#include <stdint.h>

typedef enum stackop {PUSH,POP,GET,SET,SIZE} stackop;

uint32_t stack_push(uint32_t elem);
uint32_t stack_pop();
uint32_t stack_get(uint32_t index);
uint32_t stack_set(uint32_t elem);
uint32_t stack_size();

#endif