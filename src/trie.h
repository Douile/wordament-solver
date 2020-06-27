struct Trie_t;
typedef struct Trie_t Trie_t;

#ifndef _TRIE_INCLUDED
#define _TRIE_INCLUDED

#include "wordament.h"

typedef struct Trie_t {
  bool end;
  char letter;
  struct Trie_t *children[ALPHABET_SIZE];
} Trie_t;



Trie_t * new_trie(char letter,bool end);
Trie_t * add_trie(Trie_t *parent,char letter,bool end);
void print_trie(Trie_t *node,int depth,int max_depth);
void free_trie(Trie_t *parent);

#endif
