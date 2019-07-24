#include "wordament.h"

#pragma semicolon 1

#define ALPHABET_SIZE 26
#define OFFSET 97
#define MAXLEN 20
#define NEWLINE '\n'
#define ROW 4
#define POSITIONS ROW*ROW
const unsigned short POINTS[ALPHABET_SIZE] = {2,5,3,3,1,5,4,4,2,10,6,3,4,2,2,4,10,2,2,2,4,6,6,9,5,8};

// argv globals
bool DEBUG = false;
bool VERBOOSE = false;
char * WORDLIST = "./words/words_3_9.txt";
int MIN_LENGTH = 0;

static void catch_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nCaught SIGINT exiting...\n");
    exit(EXIT_SUCCESS);
  } else if (signo == SIGSEGV) {
    printf("\nMemory error...\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc,char **argv) {
  signal(SIGINT,catch_handler);
  signal(SIGSEGV,catch_handler);
  parse_args(argc,argv);
  printf("Loading wordlist: %s\n",WORDLIST);
  FILE *file = fopen(WORDLIST,"r");
  Trie_t *head = parse_wordlist(file);
  fclose(file);
  if (DEBUG == true) {
    bool ass = head->children['a'-OFFSET]->children['s'-OFFSET]->children['s'-OFFSET]->end;
    if (ass == true) {
      printf("Dictionary test success\n");
    } else {
      printf("Dictionary test error\n");
    }
  }
  printf("Loading done\n");
  //  print_trie(head,0,4);
  size_t size = sizeof(char*)*POSITIONS;
  if (VERBOOSE == true || DEBUG == true) {
    printf("Board size: %d\n",size);
  }
  char **board = malloc(size);
  int *point_board = malloc(sizeof(int)*POSITIONS);
  for (int y=0;y<ROW;y++) {
    for (int x=0;x<ROW;x++) {
      printf("Pos (%d,%d): ",x+1,y+1);
      char *pos = get_pos(10);
      int loc = (y*ROW)+x;
      board[loc] = pos;
      point_board[loc] = calc_points(pos);
    }
  }
  if (DEBUG == true) {
    for (int y=0;y<ROW;y++) {
      for (int x=0;x<ROW;x++) {
        int pos = y*ROW+x;
        printf("[board] Checking (%d,%d)\n",x,y);
        int size = strlen(board[pos]);
        char *copy = malloc(sizeof(char)*(size+1));
        strcpy(copy,board[pos]);
        printf("%s %d - %d\n",copy,size,point_board[pos]);
        free(copy);
      }
    }
  }
  print_board(board);
  Wordlist_t *w_head = new_wordlist(NULL);
  for (int y=0;y<ROW;y++) {
    for (int x=0;x<ROW;x++) {
      unsigned int stack[POSITIONS+1];
      stack[0] = 0;
      find_words(head,board,point_board,w_head,stack,x,y);
    }
  }
  printf("Words:\n");
  Wordlist_t *node = w_head->next;
  unsigned int w_count = 0;
  while (node != NULL) {
    char *f = float_string(node->word->points);
    printf("%s - %s\n",f,node->word->word);
    free(f);
    w_count++;
    node = node->next;
  }
  printf("Found %d words\n",w_count);
  return EXIT_SUCCESS;
}

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

void print_board(char **board) {
  int maxlen=0;
  for (int i=0;i<POSITIONS;i++) {
    int l = strlen(board[i]);
    if (l > maxlen) {
      maxlen = l;
    }
  }
  if (DEBUG == true || VERBOOSE == true) printf("[trie] Max tile length: %d\n",maxlen);
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

int calc_points(const char *pos) {
  int len = strlen(pos);
  int points = 0;
  for (int i=0;i<len;i++) {
    char c = pos[i];
    if (c == '/') {
      return 20;
    } else if (c == '-') {
      return 12;
    } else {
      points += POINTS[c-OFFSET];
    }
  }
  points *= len; // may be incorret
  // multiple chars (doubles) haven't been properly analysed to find a correct way to score them
  // for now *len (2) will do but most of the time is an over-estimate
  return points;
}

int stack_size(unsigned int *stack) {
  int i = 0;
  while (i <= POSITIONS+1 && stack[i] != 0) {
    i++;
  }
  return i;
}

unsigned int stack_get(unsigned int *stack,int n) {
  unsigned int v = stack[n];
  if (v > 0) {
    return v-1;
  }
  return 0;
}

void stack_add(unsigned int *stack,int x,int y) {
  unsigned short pos = y*ROW+x+1;
  int size = stack_size(stack);
  if (size < POSITIONS+1) {
    stack[size] = pos;
    stack[size+1] = 0;
  }
}

void stack_reduce(unsigned int *stack) {
  int size = stack_size(stack);
  if (size > 1) {
    stack[size-1] = 0;
  }
}

bool stack_includes(unsigned int *stack,int x,int y) {
  unsigned int pos = y*ROW+x+1;
  for (int i=0;i<stack_size(stack);i++) {
    if (pos == stack[i]) {
      return true;
    }
  }
  return false;
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
        printf("Start char: %d\n",stack_size(stack));
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
    if (VERBOOSE == true || DEBUG == true) printf("Found word: %s\n",word_s);
    Word_t *word = new_word(word_s,points);
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
