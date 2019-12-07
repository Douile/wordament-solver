#include "flags.h"

void set_visited(unsigned int *v,int x,int y,bool visited) {
  int i = y*ROW+x;
  unsigned int n = 1;
  n = n << i;
  if (visited == true) {
    *v = *v | n;
  } else {
    *v = *v & ~n;
  }
}

bool has_visited(unsigned int *v,int x,int y) {
  int i = y*ROW+x;
  unsigned int c = 1 << i;
  unsigned int r = c & *v;
  bool visited = false;
  if (r == c) {
    visited = true;
  }
  return visited;
}
