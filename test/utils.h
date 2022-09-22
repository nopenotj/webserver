#include <stdio.h>
#define DEFAULT "\033[39m"
#define RED "\033[91m"
#define GREEN "\033[92m"
#define ORANGE "\033[93m"
#define BLUE "\033[94m"
#define MAGENTA "\033[95m"
#define CYAN "\033[96m"
#define WHITE "\033[97m"

/*
void log_err(char* str, ...) {
    printf(RED "[LOG_ERR] " DEFAULT);
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    perror(NULL);
}
*/
