#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

// Reference : https://www.codeproject.com/Articles/5329247/How-to-change-text-color-in-a-Linux-terminal
#define GREEN   "\033[92m"
#define DEFAULT "\033[39m"
#define BLACK "\033[30m"
#define DARK_RED "\033[31m"
#define DARK_GREEN "\033[32m"
#define DARK_YELLOW "\033[33m"
#define DARK_BLUE "\033[34m"
#define DARK_MAGENTA "\033[35m"
#define DARK_CYAN "\033[36m"
#define LIGHT_GRAY "\033[37m"
#define DARK_GRAY "\033[90m"
#define RED "\033[91m"
#define GREEN "\033[92m"
#define ORANGE "\033[93m"
#define BLUE "\033[94m"
#define MAGENTA "\033[95m"
#define CYAN "\033[96m"
#define WHITE "\033[97m"

void log_err(char* str, ...) {
    printf(RED "[LOG_ERR] " DEFAULT);
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    perror(NULL);
}
void log_debug(char* str, ...) {
    printf(GREEN "[LOG_DEBUG] " DEFAULT);
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}
