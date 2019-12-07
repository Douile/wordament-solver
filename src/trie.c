#include "trie.h"

Trie_t * new_trie(char letter,bool end) {
  Trie_t *node = malloc(sizeof(Trie_t)+sizeof(Trie_t *)*ALPHABET_SIZE);
  node->letter = letter;
  node->end = end;
  for (int i=0;i<ALPHABET_SIZE;i++) {
    node->children[i] = NULL;
  }
  return node;
}

Trie_t * add_trie(Trie_t *parent,char letter,bool end) {
  int pos = letter-OFFSET;
  Trie_t *node;
  if (parent->children[pos] != NULL) {
    node = parent->children[pos];
  } else {
    node = new_trie(letter,end);
    parent->children[pos] = node;
  }
  return node;
}

void print_trie(Trie_t *node,int depth,int max_depth) {
  char *output = malloc(sizeof(char)*(6+ALPHABET_SIZE*3+depth-1));
  char *pre = malloc(depth);
  for (int i=0;i<depth;i++) {
    pre[i] = '.';
  }
  pre[depth] = 0;
  sprintf(output,"%s%c -> ",pre,node->letter);
  depth++;
  for (int i=0;i<ALPHABET_SIZE;i++) {
    if (node->children[i] != NULL) {
      char *child_s = malloc(sizeof(char)*4);
      strcpy(child_s," , ");
      child_s[0] = i+OFFSET;
      strcat(output,child_s);
      free(child_s);
      if (depth < max_depth) {
        Trie_t *child = node->children[i];
        print_trie(child,depth,max_depth);
      }
    }
  }
  printf("%s\n",output);
}
