#ifndef _UTIL_INCLUDED
#define _UTIL_INCLUDED

#include "wordament.h"
#include <stdarg.h>

void parse_args(int argc,char **argv);
char * float_string(float n);
char * readline(char *wordlist, int *pos);
char * get_pos(int maxlen);
void parse_wordlist(Trie_t *head, char *wordlist);
void debug_string(const char *chars);
void debug_print(const char * format, ...);
void verbose_print(const char * format, ...);
#endif
