#ifndef _STACK_INCLUDED
#define _STACK_INCLUDED

#include "wordament.h"

int stack_size(unsigned int *stack);
unsigned int stack_get(unsigned int *stack,int n);
void stack_add(unsigned int *stack,int x,int y);
void stack_reduce(unsigned int *stack);
bool stack_includes(unsigned int *stack,int x,int y);

#endif
