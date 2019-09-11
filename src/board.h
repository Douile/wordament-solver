#ifndef _BOARD_INCLUDED
#define _BOARD_INCLUDED

#include "wordament.h"

size_t alloc_board(char **board, int pos_len);
void pos_board(char **board, char *output, int y, int x);
void print_board(char **board);
int calc_points(const char *pos);
void calc_point_board(const char **board, int *point_board);
bool valid_board(const char **board);

#endif
