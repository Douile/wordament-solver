#ifndef _UTIL_INCLUDED
#define _UTIL_INCLUDED

#include "wordament.h"

void parse_args(int argc,char **argv);
char * float_string(float n);
char * readline(FILE *file);
char * get_pos(int maxlen);
Trie_t * parse_wordlist(FILE *wordlist);
void debug_string(const char *chars);
#endif
