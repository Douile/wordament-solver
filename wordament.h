/* Includes */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define ALPHABET_SIZE 26

/* Types */

typedef enum {false, true} bool;

typedef struct Trie_t Trie_t;
typedef struct Trie_t {
  bool end;
  char letter;
  Trie_t *children[ALPHABET_SIZE];
};
typedef struct {
  char *word;
  float points;
} Word_t;
typedef struct Wordlist_t Wordlist_t;
typedef struct Wordlist_t {
  Word_t *word;
  Wordlist_t *next;
};
const int CHILDREN_SIZE = sizeof(Trie_t *)*ALPHABET_SIZE;

/* Functions */

void parse_args(int argc,char **argv);
char * float_string(float n);
Trie_t * new_trie(char letter,bool end);
Trie_t * add_trie(Trie_t *parent,char letter,bool end);
void print_trie(Trie_t *node,int depth,int max_depth);
char * readline(FILE *file);
Trie_t * parse_wordlist(FILE *wordlist);
char * get_pos(int maxlen);
void print_board(char **board);
void set_visited(unsigned int *v,int x,int y,bool visited);
bool has_visited(unsigned int *v,int x,int y);
Word_t * new_word(char *word,float points);
Wordlist_t * new_wordlist(Word_t *word);
void add_word(Wordlist_t *head,Word_t *word);
int calc_points(const char *pos);
int stack_size(unsigned int *stack);
unsigned int stack_get(unsigned int *stack,int n);
void stack_add(unsigned int *stack,int x,int y);
void stack_reduce(unsigned int *stack);
bool stack_includes(unsigned int *stack,int x,int y);
void debug_string(const char *chars);
void find_words(Trie_t *node,char **board,int *point_board,Wordlist_t *wordlist,unsigned int *stack,int ox,int oy);


