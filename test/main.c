#include "../array/array.h"
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define LEN(arr) (size_t)(sizeof(test_input) / sizeof(test_input[0]))

void array_put_get_str() {
    // Testing String arrays
    struct array a = { .esize = sizeof(char*) };
    char* test_input[] = {
	"bye world",
	"hello world",
	"",
    };
    for(int i = 0;i < LEN(test_input); i++) array_push(&a,test_input[i]);
    
    int passed = 0;
    char *actual, *expected;
    for(int i = 0;i < LEN(test_input); i++) {
	 actual = array_get(a,i,char*);
	 expected = test_input[i];

	 if(strcmp(actual,expected) == 0) {
	     passed++;
	 } else {
	     printf("[TEST CASE %d] %s != %s",i,actual,expected);
	 }
    }
    printf(GREEN"[Array] %d/%zu PASSED.\n"DEFAULT, passed, LEN(test_input));
}

int main() {
    // Array Tests
    array_put_get_str();

    return 0;
}
