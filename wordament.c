#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

// switch to lower case because word list is lower case
#define ALPHABET_SIZE 26
#define OFFSET 97
#define WORDLIST "./words/words_3_9.txt"
#define MAXLEN 20
#define NEWLINE '\n'
#define ROW 4
typedef enum {false, true} bool;

static void catch_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nCaught SIGINT exiting...\n");
    exit(EXIT_SUCCESS);
  } else if (signo == SIGSEGV) {
    printf("\nMemory error...\n");
    exit(EXIT_FAILURE);
  }
}

typedef struct Trie_t Trie_t;
typedef struct Trie_t {
  bool end;
  char letter;
  Trie_t *children[ALPHABET_SIZE];
};
typedef struct Wordlist_t Wordlist_t;
typedef struct Wordlist_t {
  char *word;
  Wordlist_t *next;
};
typedef struct {
  char *chars;
  int size;
} Charstack_t;

const int CHILDREN_SIZE = sizeof(Trie_t *)*ALPHABET_SIZE;

Trie_t * new_trie(char letter,bool end) {
  // doesn't work but did before when not in function
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
  line = realloc(line,size);
  return line;
}

Trie_t * parse_wordlist(FILE *wordlist) {
  size_t output_pos = ftell(stdout);
  fseek(wordlist,0L,SEEK_END);
  size_t size = ftell(wordlist);
  fseek(wordlist,0L,SEEK_SET);
  Trie_t *head = new_trie('*',false);
  while (ftell(wordlist) < size) {
    char *word = readline(wordlist);
    //sprintf(word,"%s",word);
    if (strlen(word) < 1) {
      continue;
    }
    // printf("%s",word);
    // fseek(stdout,-1*strlen(word),SEEK_CUR);
    // printf("%s\n",word);
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
  //printf("%s\n",pos);
  return pos;
}

void print_board(char **board) {
  int maxlen=0;
  int size = ROW*ROW;
  for (int i=0;i<size;i++) {
    int l = strlen(board[i]);
    if (l > maxlen) {
      maxlen = l;
    }
  }
  //printf("Max length: %d\n",maxlen);
  int row_size = sizeof(char)*((ROW*(maxlen+1))+1);
  for (int y=0;y<ROW;y++) {
    char *row = malloc(row_size);
    for (int x=0;x<ROW;x++) {
      int p = ROW*y+x;
      int offset = (maxlen+1)*x;
      int len = strlen(board[p]);
      for (int t=0;t<maxlen+1;t++) {
        if (t < len) {
          row[offset+t] = board[p][t];
        } else {
          row[offset+t] = ' ';
        }
      }
    }
    row[row_size] = 0;
    printf("%s\n",row);
    free(row);
  }
}

void set_visited(unsigned int *v,int x,int y,bool visited) {
  int i = y*ROW+x;
  unsigned int n = 1;
  n = n << i;
  if (visited == true) {
    *v = *v | n;
  } else {
    *v = *v & ~n;
  }
}

bool has_visited(unsigned int *v,int x,int y) {
  int i = y*ROW+x;
  unsigned int c = 1 << i;
  unsigned int r = c & *v;
  bool visited = false;
  if (r == c) {
    visited = true;
  }
  return visited;
}

Wordlist_t * new_wordlist(char *word) {
  Wordlist_t *wordlist = NULL;
  wordlist = malloc(sizeof(Wordlist_t));
  wordlist->word = word;
  wordlist->next = NULL;
  return wordlist;
}

void add_word(Wordlist_t *head,char *word) {
  Wordlist_t *new_word = new_wordlist(word);
  Wordlist_t *node = head;
  while (node->next != NULL) {
    node = node->next;
    if (strcmp(node->word,word) == 0) {
      return;
    }
  }
  node->next = new_word;
}

// Wordlist_t * new_wordlist() {
//   Wordlist_t *words = malloc(sizeof(Wordlist_t));
//   words->size = 0;
//   return words;
// }
//
// void add_word(Wordlist_t *words,char *word) {
//   int size = words->size + 1;
//   words->size = size;
//   words = realloc(words,sizeof(Wordlist_t)+sizeof(char*)*(size+1));
//   words->words[size] = word;
//   printf("New word:%s\n",word);
// }
//
// Charstack_t * new_charstack() {
//   Charstack_t *stack = malloc(sizeof(Charstack_t)+sizeof(char));
//   stack->size = 1;
//   stack->chars[0] = 0;
//   return stack;
// }
// void add_char(Charstack_t *stack,char c) {
//   int size = stack->size+1;
//   stack->size = size;
//   stack = realloc(stack,sizeof(Charstack_t)+sizeof(char)*size);
//   stack->chars[size-1] = c;
//   stack->chars[size] = 0;
//   printf("%s\n",stack->chars);
// }
//
// void reduce_char(Charstack_t *stack) {
//   int size = stack->size-1;
//   stack->size = size;
//   //stack->chars[size+1] = NULL;
//   stack->chars[size] = 0;
//   stack = realloc(stack,sizeof(Charstack_t)+sizeof(char)*size);
// }
void add_char(char *stack,char c) {
  size_t size = strlen(stack);
  //stack = realloc(stack,sizeof(char)*(size+1));
  stack[size] = c;
  stack[size+1] = 0;
}

void reduce_char(char *stack) {
  size_t size = strlen(stack);
  if (size > 0) {
    //stack = realloc(stack,sizeof(char)*(size-1));
    stack[size-1] = 0;
  }
}

void find_words(Trie_t *node,char **board,Wordlist_t *wordlist,char *stack,unsigned int *visited,int ox,int oy) {
  if (has_visited(visited,ox,oy) == true) return;
  char *c_string = board[ROW*oy+ox];
  for (int i=0;i<strlen(c_string);i++) {
    char c = c_string[i];
    // add handling for x/y, -start and -end
    if (node->children[c-OFFSET] != NULL) {
      node = node->children[c-OFFSET];
      add_char(stack,c);
      //printf("Built: %s\n",stack);
    } else {
      for (int j=0;j<i;j++) {
        reduce_char(stack);
      }
      return;
    }
  }
  set_visited(visited,ox,oy,true);
  if (node->end == true) {
    size_t size = strlen(stack);
    char *word = malloc(sizeof(char)*size);
    strcpy(word,stack);
    //printf("%s\n",word);
    add_word(wordlist,word);
  }
  for (int dx=-1;dx<=1;dx++) {
    int x = ox+dx;
    if (x < 0 || x >= ROW) continue;
    for (int dy=-1;dy<=1;dy++) {
      int y = oy+dy;
      if (y < 0 || y >= ROW) continue;
      if (dy == 0 && dx == 0) continue;
      find_words(node,board,wordlist,stack,visited,x,y);
    }
  }
  set_visited(visited,ox,oy,false);
  for (int i=0;i<strlen(c_string);i++) {
    reduce_char(stack);
  }
}


int main(int argc,char **argv) {
  signal(SIGINT,catch_handler);
  signal(SIGSEGV,catch_handler);
  char* wordlist = NULL;
  bool debug = false;
  bool verboose = false;
  int minlen = 0;
  for (int i=1;i<argc;i++) {
    char *arg = argv[i];
    if (strcmp(arg,"-d") == 0 || strcmp(arg,"--debug") == 0) {
      debug = true;
    } else if (strcmp(arg,"-v") == 0 || strcmp(arg,"--verboose") == 0) {
      verboose = true;
    } else if (strcmp(arg,"-m") == 0 || strcmp(arg,"--min-len") == 0) {
      if (i+1 < argc) {
        i++;
        minlen = atoi(argv[i]);
      }
    } else if (strcmp(arg,"-h") == 0 || strcmp(arg,"--help") == 0) {
      printf("Usage: %s [-h] [-v] [-d] [-m n] ...wordlist\n\n  -h, --help = Print this help message\n  -v, --verboose = Run in verboose mode\n  -d, --debug = Run in debug mode\n  -m [n],--min-len [n] = set the min length for words to be found to [n]\n  wordlist = file to load wordlist from\n\n",argv[0]);
      return 0;
    } else {
      wordlist = arg;
    }
  }
  if (wordlist == NULL) {
    wordlist = WORDLIST;
  }
  printf("Loading wordlist: %s\n",wordlist);
  FILE *file = fopen(wordlist,"r");
  Trie_t *head = parse_wordlist(file);
  fclose(file);
  if (debug == true) {
    bool ass = head->children['a'-OFFSET]->children['s'-OFFSET]->children['s'-OFFSET]->end;
    if (ass == true) {
      printf("Ass\n");
    } else {
      printf("No ass\n");
    }
  }
  printf("Loading done\n");
  //  print_trie(head,0,4);
  size_t size = sizeof(char*)*ROW*ROW;
  if (verboose == true || debug == true) {
    printf("Board size: %d\n",size);
  }
  char **board = malloc(size);
  for (int y=0;y<ROW;y++) {
    for (int x=0;x<ROW;x++) {
      printf("Pos (%d,%d): ",x+1,y+1);
      char *pos = get_pos(10);
      int loc = (y*ROW)+x;
      board[loc] = pos;
    }
  }
  print_board(board);
  Wordlist_t *w_head = new_wordlist("");
  for (int y=0;y<ROW;y++) {
    for (int x=0;x<ROW;x++) {
      unsigned int *visited = malloc(sizeof(unsigned int));
      *visited = 0;
      //Charstack_t *stack = new_charstack();
      char stack[ROW*ROW];
      stack[0] = 0;
      find_words(head,board,w_head,stack,visited,x,y);
    }
  }
  printf("Words:\n");
  Wordlist_t *node = w_head->next;
  unsigned int w_count = 0;
  while (node != NULL) {
    if (minlen > 0) {
      if (strlen(node->word) >= minlen) {
        printf("%s\n",node->word);
        w_count++;
      }
    } else {
      printf("%s\n",node->word);
      w_count++;
    }
    node = node->next;
  }
  printf("Found %d words\n",w_count);
  //printf("Found %d words\n",wordlist->size);
  // for (int i=0;i<wordlist->size;i++) {
  //   printf("%s\n",wordlist->words[i]);
  // }
  return EXIT_SUCCESS;
}
