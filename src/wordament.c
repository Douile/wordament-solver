#include "wordament.h"
#include <assert.h>

#define MAX_POS_LENGTH 10

bool VERBOOSE = false;
char * WORDLIST = "./words/words_3_9.txt";
int MIN_LENGTH = 0;

char status_wordlist[128] = "Loading wordlist...";
bool b_echo = true;
char **board;
int *point_board;
Trie_t *wl_head;
int board_y, board_x;
Wordlist_t *wordout_head;

static void catch_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nCaught SIGINT exiting...\n");
    exit(EXIT_SUCCESS);
  } else if (signo == SIGSEGV) {
    printf("\nMemory error...\n");
    exit(EXIT_FAILURE);
  }
}

/** Steps
*******************************
* Parse argv
* (Load wordlist into trie)
* Setup board
* Input characters into board
* Find words
* Print words
*/

int main(int argc, char **argv) {
	strcpy(status_wordlist, "Loading wordlist...");
	parse_args(argc, argv);
	WINDOW *w = init_curses();
	setup_color();

	board = malloc(sizeof(char*)*POSITIONS);
	size_t size = alloc_board(board, MAX_POS_LENGTH);
	point_board = malloc(sizeof(int)*POSITIONS);

	assert(size == sizeof(char)*POSITIONS*(MAX_POS_LENGTH+1));
	// fprintf(stderr, "BOARD(%d) %d x %d =  %d", (int)board, sizeof(char*), POSITIONS, size);

	draw_curses();
	FILE *f_wl = fopen(WORDLIST, "r");
	wl_head = parse_wordlist(f_wl);
	fclose(f_wl);
	sprintf(status_wordlist, "Loaded wordlist %s", WORDLIST);

	loop_curses();
	kill_curses(0);
}

/*
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
*/

int find_words(Trie_t *init_node,char **board,int *point_board,Wordlist_t *wordlist,unsigned int *stack,int ox,int oy) {
  if (stack_includes(stack,ox,oy) == true) return 0;
  char *c_string = board[ROW*oy+ox];
  int len = strlen(c_string), count = 0;
  bool end = false;
  Trie_t *node = init_node;
  for (int i=0;i<len;i++) {
    char c = c_string[i];
    if (c == '-') {
      if (i == 0) {
        end = true;
      } else {
        if (stack_size(stack) > 0) return 0;
        sprintf(status_wordlist, "Start char: %d\n",stack_size(stack));
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
      #if DEBUG
      char buf[32];
      sprintf(buf, "Node %c = Node[%p] %c", c, node->letter);
      log_tf(buf);
      #endif
    } else {
      return 0;
    }
  }
  stack_add(stack,ox,oy);
  #if DEBUG
  assert(node != NULL);
  #endif
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
					word_s[x] = pos[j]; x++;
				}
			}
		}
		word_s[x] = 0;
		word_s = realloc(word_s,sizeof(char)*(x+1));
		if (VERBOOSE == true || DEBUG == true) printf("Found word: %s\n",word_s);
		Word_t *word = new_word(word_s,points);
		add_word(wordlist,word);
		count+= 1;
	}
	if (end == false) {
		for (int dx=-1;dx<=1;dx++) {
			int x = ox+dx;
			if (x < 0 || x >= ROW) continue;
			for (int dy=-1;dy<=1;dy++) {
				int y = oy+dy;
				if (y < 0 || y >= ROW) continue;
				if (dy == 0 && dx == 0) continue;
      	count += find_words(node,board,point_board,wordlist,stack,x,y);
			}
		}
	}
	for (int i=0;i<strlen(c_string);i++) {
		stack_reduce(stack);
  }
	return count;
}

void register_signals() {
	signal(SIGINT, kill_curses);
	signal(SIGTERM, kill_curses);
	signal(SIGKILL, kill_curses);
}

void kill_curses(int code) {
	endwin();
	printf("Goodbye\n");
	exit(code);
}

WINDOW * init_curses() {
  register_signals();

  WINDOW *wnd;
  wnd = initscr();
  cbreak();
  noecho();
  curs_set(0);
  clear();
  refresh();

	board_y = 0;
	board_x = 0;
  return wnd;
}

void loop_curses() {
	while(1) {
		draw_curses();
		input_handler();
	}
}

void draw_curses() {
	clear();
	move(0,0);
	insstr(status_wordlist);
	int oy = 3, ox = 2;
	// move(oy,ox);
	char spos[MAX_POS_LENGTH+1];
	spos[0] = 0;
	int blen = MAX_POS_LENGTH+6;
	for (int y=0;y<ROW;y++) {
		for (int x=0;x<ROW;x++) {
			move(oy + y, ox + x * blen);
			if (x == board_x && y == board_y) {
				attron(A_BOLD|COLOR_PAIR(PAIR_BW));
			}
			int ipos = (ROW*y)+x;
			// char *remote = board[ipos];
			// strcpy(spos, remote);
			pos_board(board, spos, y, x);
			pad_string(spos, blen-1);
			char points[10];
			sprintf(points,"[%d]",point_board[ipos]);
			spos[blen-2-strlen(points)] = 0;
			char out[64];
			sprintf(out,"%s%s",spos,points);
			insstr(out);
			attroff(A_BOLD|COLOR_PAIR(PAIR_BW));
		}
	}
	refresh();
}

void input_handler() { /* TODO: put some longer handlers in their own function */
	char i = getch();
	switch(i) {
	case 'Q':
		kill_curses(0);
		break;
	case 'W':
		if (valid_board(board)) {
      // strcpy(status_wordlist, "Valid board");
    	Trie_t *head = wl_head;
      if (test_trie(head)) {
        calc_point_board(board, point_board);
        Wordlist_t *w_head = new_wordlist(NULL);
        for (int y=0;y<ROW;y++) {
          for (int x=0;x<ROW;x++) {
            unsigned int stack[POSITIONS+1];
            stack[0] = 0;
            find_words(head,board,point_board,w_head,stack,x,y);
          }
        }
        size_t size = wordlist_size(w_head);
        #if DEBUG
        char buf[32];
        sprintf(buf, "Found %d words", size);
        log_tf(buf);
        #endif
        endwin();
        printf("Found %d words:\n", size);
        while(w_head->next !=NULL) {
          w_head = w_head->next;
          printf("%s\n", w_head->word->word);
        }
        /* TODO: Output words in an interactive list similar to less */
        kill_curses(0);
      } else {
        strcpy(status_wordlist, "Invalid wordlist");
      }
    } else {
      strcpy(status_wordlist, "Invalid board");
    }
		break;
	case '\033':
		getch();
		board_move(getch());
		break;
	default:
		if (( i >= 'a' && i <= 'z') || i == '/' || i == '-') {
			int ipos = board_y*ROW + board_x;
			int l = strlen(board[ipos]);
			if (l<MAX_POS_LENGTH) {
				board[ipos][l] = i;
				board[ipos][l+1] = 0;
			}
			// sprintf(board[ipos], "%s%c", board[ipos], i);
			point_board[ipos] = calc_points(board[ipos]);
		} else if (i == 127 || i == 8) {
			int ipos = board_y*ROW + board_x;
			int l = strlen(board[ipos]);
			if (l > 0) {
				board[ipos][l -1 ] = 0;
				point_board[ipos] = calc_points(board[ipos]);
			}
		}
		break;
	}
}

void board_move(char direction) {
	switch(direction) {
		case 'A': /* UP */
		if (board_y > 0) board_y += -1;
		break;
		case 'B': /* DOWN */
		if (board_y < ROW-1) board_y += 1;
		break;
		case 'C': /* RIGHT */
		if (board_x < ROW-1) board_x += 1;
		break;
		case 'D': /* LEFT */
		if (board_x > 0) board_x += -1;
		break;
	}
}

void display_words(Wordlist_t *head) {

}
