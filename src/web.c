#include "wordament.h"
#include "emscripten.h"

#define STATE_WORDLIST 1
#define STATE_BOARD 1 << 1

int STATE = 0;
Trie_t * ACTIVE_WORDLIST;
char ** ACTIVE_BOARD;
int * ACTIVE_POINTS;

EMSCRIPTEN_KEEPALIVE
int version() {
  return VERSION;
}

EMSCRIPTEN_KEEPALIVE
int get_state() {
  return STATE;
}

EMSCRIPTEN_KEEPALIVE
int load_wordlist(char *wordlist) {
  if (wordlist == NULL) {
    printf("No Wordlist provided\n");
    return 1;
  }
  if (ACTIVE_WORDLIST != NULL) {
    free_trie(ACTIVE_WORDLIST);
  }

  ACTIVE_WORDLIST = parse_wordlist(wordlist);
  STATE |= STATE_WORDLIST;
  return 0;
}

EMSCRIPTEN_KEEPALIVE
int print_wordlist(int max_depth) {
  if (ACTIVE_WORDLIST == NULL || (STATE & STATE_WORDLIST) == 0) {
    printf("No active wordlist\n");
    return 1;
  }
  print_trie(ACTIVE_WORDLIST, 0, max_depth);
  return 0;
}

EMSCRIPTEN_KEEPALIVE
int load_board(char *board) {
  if (board == NULL) {
    printf("No board provided\n");
    return 1;
  }

  if (ACTIVE_BOARD != NULL) {
    free(ACTIVE_BOARD);
  }
  if (ACTIVE_POINTS != NULL) {
    free(ACTIVE_POINTS);
  }

  ACTIVE_BOARD = malloc(sizeof(char *)*POSITIONS);
  ACTIVE_POINTS = malloc(sizeof(int)*POSITIONS);

  int pos = 0;
  for (int i=0;i<POSITIONS;i++) {
    ACTIVE_BOARD[i] = readline(board, &pos);
    ACTIVE_POINTS[i] = calc_points(ACTIVE_BOARD[i]);
  }

  STATE |= STATE_BOARD;
  return 0;
}

EMSCRIPTEN_KEEPALIVE
Wordlist_t * search() {
  if (
    (STATE & STATE_BOARD) == 0 ||
    (STATE & STATE_WORDLIST) == 0
  ) {
    printf("Must load a wordlist and board before searching\n");
    return NULL;
  }
  Wordlist_t *w_head = new_wordlist(NULL);
  for (int y=0;y<ROW;y++) {
    for (int x=0;x<ROW;x++) {
      unsigned int stack[POSITIONS+1];
      stack[0] = 0;
      find_words(ACTIVE_WORDLIST,ACTIVE_BOARD,ACTIVE_POINTS,w_head,stack,x,y);
    }
  }
  return w_head;
}
