#include "util.h"

void parse_args(int argc,char **argv) {
  char *wordlist = NULL;
  for (int i=1;i<argc;i++) {
    char *arg = argv[i];
    if (strcmp(arg,"-d") == 0 || strcmp(arg,"--debug") == 0) {
      DEBUG = true;
      printf("Debug mode enabled...\n");
    } else if (strcmp(arg,"-v") == 0 || strcmp(arg,"--verboose") == 0) {
      VERBOOSE = true;
      printf("Verboose mode enabled...\n");
    } else if (strcmp(arg,"-m") == 0 || strcmp(arg,"--min-len") == 0) {
      if (i+1 < argc) {
        i++;
        MIN_LENGTH = atoi(argv[i]);
        printf("Min length set to %d\n",MIN_LENGTH);
      }
    } else if (strcmp(arg,"-h") == 0 || strcmp(arg,"--help") == 0) {
      printf("Usage: %s [-h] [-v] [-d] [-m n] ...wordlist\n\n  -h, --help = Print this help message\n  -v, --verboose = Run in verboose mode\n  -d, --debug = Run in debug mode\n  -m [n],--min-len [n] = set the min length for words to be found to [n]\n  wordlist = file to load wordlist from\n\n",argv[0]);
      exit(EXIT_SUCCESS);
    } else {
      wordlist = arg;
    }
  }
  if (wordlist != NULL) {
    WORDLIST = wordlist;
  }
}

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
  if (DEBUG == true) printf("[fstring] %f -> %s\n",n,text);
  return text;
}

char * readline(FILE *file) {
  int i = 0;
  char *line = malloc(sizeof(char)*MAXLEN);
  char c;
  do {
    c = fgetc(file);
    line[i] = c;
    i++;
  } while (c != NEWLINE && c != 0 && c != EOF);
  line[i-1] = 0;
  size_t size = sizeof(char)*i;
  if (i-2 < MIN_LENGTH) {
    free(line);
    line = NULL;
  } else {
    line = realloc(line,size);
  }
  return line;
}

char * get_pos(int maxlen) {
  int i = 0;
  char *pos = malloc(sizeof(char)*(maxlen+1));
  char c;
  do {
    c = fgetc(stdin);
    if (c >= 'A' && c <= 'Z') {
      pos[i] = c+32; // convert to lower case
      i++;
    } else if ((c >= 'a' && c <= 'z') || c == '-' || c == '/') { // 'a'/'z' or '-' or '/'
      pos[i] = c;
      i++;
    } else {
      break;
    }
  } while(i < maxlen);
  pos[i] = 0;
  if (i < 1) {
    free(pos);
    printf("No entry: ");
    char *new_pos = get_pos(maxlen);
    pos = new_pos;
  } else {
    size_t size = sizeof(char)*(i+1);
    pos = realloc(pos,size);
  }
  return pos;
}

Trie_t * parse_wordlist(FILE *wordlist) {
  size_t output_pos = ftell(stdout);
  fseek(wordlist,0L,SEEK_END);
  size_t size = ftell(wordlist);
  fseek(wordlist,0L,SEEK_SET);
  Trie_t *head = new_trie('*',false);
  while (ftell(wordlist) < size) {
    char *word = readline(wordlist);
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
  }
  printf("\n");
  return head;
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
  printf("  <-- Length: %d\n",i);
}
