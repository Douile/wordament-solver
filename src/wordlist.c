#include "wordlist.h"

Word_t * new_word(char *word,float points,unsigned int *stack) {
  Word_t * node = malloc(sizeof(Word_t));
  node->word = word;
  node->points = points;
  node->stack = stack_copy(stack);
  return node;
}

Wordlist_t * new_wordlist(Word_t *word) {
  Wordlist_t *wordlist = NULL;
  wordlist = malloc(sizeof(Wordlist_t));
  wordlist->word = word;
  wordlist->next = NULL;
  return wordlist;
}

void add_word(Wordlist_t *head,Word_t *word) {
  Wordlist_t *new_word = new_wordlist(word);
  Wordlist_t *node = head;
  // 2 loops inefficient but only using 1 loop wasn't working
  while (node->next != NULL) {
    node = node->next;
    if (strcmp(word->word,node->word->word) == 0 && word->points == node->word->points) {
      debug_print("Duplicate word %s\n",word->word);
      free(word->word);
      free(word);
      free(new_word);
      return;
    }
  }
  node = head;
  while (node->next != NULL) {
    if (isgreater(word->points,node->next->word->points)) break;
    node = node->next;
  }
  if (node->next == NULL) {
    node->next = new_word;
    debug_print("[wlist] Appended word %s\n",new_word->word->word);
  } else {
    new_word->next = node->next;;
    node->next = new_word;
    debug_print("[wlist] Inserted word %s\n",new_word->word->word);
  }
}

void free_wordlist(Wordlist_t *wordlist) {
  if (wordlist->next != NULL) {
    free_wordlist(wordlist->next);
  }
  if (wordlist->word != NULL) {
    if (wordlist->word->word != NULL) free(wordlist->word->word);
    if (wordlist->word->stack != NULL) free(wordlist->word->stack);
    free(wordlist->word);
  }
  free(wordlist);
}
