#ifndef _WORDAMENT_INCLUDED
#define _WORDAMENT_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef enum {false, true} bool;

#define ALPHABET_SIZE 26
#define OFFSET 97
#define MAXLEN 20
#define NEWLINE '\n'
#define ROW 4
#define POSITIONS ROW*ROW

extern bool DEBUG;
extern bool VERBOOSE;
extern char * WORDLIST;
extern int MIN_LENGTH;
// argv globals
// bool DEBUG = false;
// bool VERBOOSE = false;
// char * WORDLIST = "./words/words_3_9.txt";
// int MIN_LENGTH = 0;

#include "trie.h"
#include "wordlist.h"
#include "flags.h"
#include "stack.h"
#include "board.h"
#include "util.h"

void find_words(Trie_t *node,char **board,int *point_board,Wordlist_t *wordlist,unsigned int *stack,int ox,int oy);

#endif
