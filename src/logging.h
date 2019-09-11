#ifndef _LOG_INCLUDED
#define _LOG_INCLUDED

#include "wordament.h"
#include <time.h>

#define LOG_FILE "debug.log"
#define TIME_BUFFER_SIZE 30

void log_tf(char *message);
void log_time(char *buffer, int length);

#endif
