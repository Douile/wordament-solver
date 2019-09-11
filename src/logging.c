#include "logging.h"

void log_tf(char *message) {
  char buffer[TIME_BUFFER_SIZE];
  log_time(buffer, TIME_BUFFER_SIZE);

  FILE *f = fopen(LOG_FILE, "a");
  fprintf(f, "%s :: %s\n", buffer, message);
  fclose(f);
}

void log_time(char *buffer, int length) {
  time_t timer;
  struct tm* tm_info;

  time(&timer);
  tm_info = localtime(&timer);

  strftime(buffer, length, "%Y-%m-%d %H:%M:%S", tm_info);
}
