#include "stack.h"

int stack_size(unsigned int *stack) {
  int i = 0;
  while (i <= POSITIONS+1 && stack[i] != 0) {
    i++;
  }
  return i;
}

unsigned int stack_get(unsigned int *stack,int n) {
  unsigned int v = stack[n];
  if (v > 0) {
    return v-1;
  }
  return 0;
}

void stack_add(unsigned int *stack,int x,int y) {
  unsigned short pos = y*ROW+x+1;
  int size = stack_size(stack);
  if (size < POSITIONS+1) {
    stack[size] = pos;
    stack[size+1] = 0;
  }
}

void stack_reduce(unsigned int *stack) {
  int size = stack_size(stack);
  if (size > 1) {
    stack[size-1] = 0;
  }
}

bool stack_includes(unsigned int *stack,int x,int y) {
  unsigned int pos = y*ROW+x+1;
  for (int i=0;i<stack_size(stack);i++) {
    if (pos == stack[i]) {
      return true;
    }
  }
  return false;
}
