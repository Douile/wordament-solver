#include "util.h"

char * float_string(float n) {
  char *text = malloc(sizeof(char)*20);
  sprintf(text,"%f",n);
  int len = strlen(text);
  for (int i=1;i<=len;i++) {
    int x = len-i;
    if (text[x] == '0') {
      text[x] = 0;
    } else if (text[x] == '.') {
      text[x] = 0;
      break;
    } else {
      break;
    }
  }
  int n_len = strlen(text)+1;
  text = realloc(text,sizeof(char)*n_len);
  debug_print("[fstring] %f -> %s\n",n,text);
  return text;
}

char * readline(char *wordlist, int *pos) {
  int i = 0;
  char *line = malloc(sizeof(char)*MAXLEN);
  char c;
  do {
    c = wordlist[(*pos)++];
    line[i++] = c;
  } while (c != NEWLINE && c != 0 && c != EOF);
  line[i-1] = 0;
  size_t size = sizeof(char)*i;
  if (i-2 < MIN_LENGTH) {
    free(line);
    line = NULL;
  } else {
    line = realloc(line,size);
  }
  verbose_print("[readline] read: %s\n", line);
  return line;
}

void parse_wordlist(Trie_t *head, char *wordlist) {
  size_t size = strlen(wordlist);
  int pos = 0;
  if (head == NULL) {
    head = new_trie('*',false);
  }
  verbose_print("[wparse] loading from %d chars\n", size);
  while (pos < size) {
    char *word = readline(wordlist, &pos);
    if (word == NULL) continue;
    Trie_t *node = head;
    for (int i=0;i<strlen(word);i++) {
      char c = word[i];
      int x = c-OFFSET;
      if (x >= 0 && x <= ALPHABET_SIZE) {
        node = add_trie(node,c,false);
      }
    }
    node->end = true;
    verbose_print("[wparse] loaded %s\n", word);
    free(word);
  }
}

void debug_string(const char *chars) {
  int i = 0;
  while (i<20) {
    char c = chars[i];
    i++;
    printf("%c:%d,",c,c);
    if (c == 0) {
      break;
    }
  }
  debug_print("  <-- Length: %d\n",i);
}

void debug_print(const char * format, ...) {
  if (DEBUGGING >= FLAG_DEBUG) {
    va_list argptr;
    va_start(argptr,format);
    vprintf(format, argptr);
    va_end(argptr);
  }
}

void verbose_print(const char * format, ...) {
  if (DEBUGGING >= FLAG_VERBOSE) {
    va_list argptr;
    va_start(argptr,format);
    vprintf(format, argptr);
    va_end(argptr);
  }
}
