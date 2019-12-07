#include "wordlist.h"

Word_t * new_word(char *word,float points) {
  Word_t * node = malloc(sizeof(Word_t));
  node->word = word;
  node->points = points;
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
      if (VERBOOSE == true || DEBUG == true) printf("Duplicate word %s\n",word->word);
      free(word->word);
      free(word);
      free(new_word);
      return;
    }
  }
  node = head;
  while (node->next != NULL) {
    if (word->points <= node->next->word->points) break;
    node = node->next;
  }
  if (node->next == NULL) {
    node->next = new_word;
    if (DEBUG == true) printf("[wlist] Appended word %s\n",new_word->word->word);
  } else {
    new_word->next = node->next;;
    node->next = new_word;
    if (DEBUG == true) printf("[wlist] Inserted word %s\n",new_word->word->word);
  }
}
