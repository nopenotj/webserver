#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

#define GREEN   "\033[92m"
#define DEFAULT "\033[39m"

void log_err(char* str, ...) {
    printf("%s",str);
    perror(NULL);
}
void log_debug(char* str, ...) {
    printf(GREEN"[LOG_DEBUG] "DEFAULT);
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}
