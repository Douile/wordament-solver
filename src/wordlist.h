typedef struct Word_t Word_t;
typedef struct Wordlist_t Wordlist_t;

#ifndef _WORDLIST_INCLUDED
#define _WORDLIST_INCLUDED

#include "wordament.h"
#include "math.h"

struct Word_t {
  char *word;
  float points;
};
typedef struct Word_t Word_t;

struct Wordlist_t {
  struct Word_t *word;
  struct Wordlist_t *next;
};
typedef struct Wordlist_t Wordlist_t;

Word_t * new_word(char *word,float points);
Wordlist_t * new_wordlist(Word_t *word);
void add_word(Wordlist_t *head,Word_t *word);
void free_wordlist(Wordlist_t *wordlist);

#endif
