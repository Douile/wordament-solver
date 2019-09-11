#ifndef _WORDAMENT_INCLUDED
#define _WORDAMENT_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <curses.h>

#define ALPHABET_SIZE 26
#define OFFSET 97
#define MAXLEN 20
#define NEWLINE '\n'
#define ROW 4
#define POSITIONS ROW*ROW
#define DEBUG 1

extern bool VERBOOSE;
extern char * WORDLIST;
extern int MIN_LENGTH;
// argv globals
//  bool DEBUG = false;
//  bool VERBOOSE = false;
//  char * WORDLIST = "./words/words_3_9.txt";
//  int MIN_LENGTH = 0;

extern char status_wordlist[128];

#if DEBUG
#include <assert.h>
#include "logging.h"
#endif

#include "trie.h"
#include "wordlist.h"
#include "flags.h"
#include "stack.h"
#include "board.h"
#include "color.h"
#include "util.h"


int find_words(Trie_t *node,char **board,int *point_board,Wordlist_t *wordlist,unsigned int *stack,int ox,int oy);
void register_signals();
void kill_curses(int code);
WINDOW * init_curses();
void loop_curses();
void draw_curses();
void input_handler();
void board_move(char direction);
void display_words(Wordlist_t *head);

#endif
