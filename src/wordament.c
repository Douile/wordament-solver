#include "wordament.h"

int DEBUGGING = 0;
int MIN_LENGTH = 0;

void find_words(Trie_t *node,char **board,int *point_board,Wordlist_t *wordlist,unsigned int *stack,int ox,int oy) {
  if (stack_includes(stack,ox,oy) == true) return;
  char *c_string = board[ROW*oy+ox];
  int len = strlen(c_string);
  bool end = false;
  for (int i=0;i<len;i++) {
    char c = c_string[i];
    if (c == '-') {
      if (i == 0) {
        end = true;
      } else {
        if (stack_size(stack) > 0) return;
        debug_print("Start char: %d\n",stack_size(stack));
      }
      continue;
    } else if (c == '/') {
      // yuck ugly code here
      // flag position with 16 (1 << 4)
      // if position > 15 then check flag and remove (less than 15: chars before /, greater than 15: remove flag then chars after /)
      continue;
    }
    if (node->children[c-OFFSET] != NULL) {
      node = node->children[c-OFFSET];
    } else {
      return;
    }
  }
  stack_add(stack,ox,oy);
  if (node->end == true) {
    int size = stack_size(stack);
    int char_count = 1;
    float points = 0.0;
    for (int i=0;i<size;i++) {
      unsigned int pos = stack_get(stack,i);
      //printf("%d:\n",pos);
      //debug_string(board[pos]);
      char_count += strlen(board[pos]);
      points += (float) point_board[pos];
    }
    if (char_count >= 8) {
      points *= 2.5;
    } else if (char_count >= 6) {
      points *= 2;
    } else if (char_count >= 5) {
      points *= 1.5;
    }
    char *word_s = malloc(sizeof(char)*char_count);
    int x = 0;
    for (int i=0;i<size;i++) {
      char *pos = board[stack_get(stack,i)];
      for (int j=0;j<strlen(pos);j++) {
        if (pos[j] != '-') {
          word_s[x] = pos[j];
          x++;
        }
      }
    }
    word_s[x] = 0;
    word_s = realloc(word_s,sizeof(char)*(x+1));
    Word_t *word = new_word(word_s,points,stack);
    if (is_debug()) {
      debug_print("[search] Found word: %s (stack size %d:%d)\n",word_s,stack_size(stack),stack_size(word->stack));
    }
    add_word(wordlist,word);
  }
  if (end == false) {
    for (int dx=-1;dx<=1;dx++) {
      int x = ox+dx;
      if (x < 0 || x >= ROW) continue;
      for (int dy=-1;dy<=1;dy++) {
        int y = oy+dy;
        if (y < 0 || y >= ROW) continue;
        if (dy == 0 && dx == 0) continue;
        find_words(node,board,point_board,wordlist,stack,x,y);
      }
    }
  }
  for (int i=0;i<strlen(c_string);i++) {
    stack_reduce(stack);
  }
}
