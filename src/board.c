#include "board.h"

const unsigned short POINTS[ALPHABET_SIZE] = {2,5,3,3,1,5,4,4,2,10,6,3,4,2,2,4,10,2,2,2,4,6,6,9,5,8};

void print_board(char **board) {
  int maxlen=0;
  for (int i=0;i<POSITIONS;i++) {
    int l = strlen(board[i]);
    if (l > maxlen) {
      maxlen = l;
    }
  }
  debug_print("[board] Max tile length: %d\n",maxlen);
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
