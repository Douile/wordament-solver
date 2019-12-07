#include "wordament.h"

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
