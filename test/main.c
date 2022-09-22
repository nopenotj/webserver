#include "../array/array.h"
#include "../dict/dict.h"
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include <assert.h>

#define LEN(arr) (size_t)(sizeof(test_input) / sizeof(test_input[0]))

void test_array_put_get_str() {
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

// TODO: alot of warnings when dealing with void* to int conversion
void test_dict_put_item() {
    struct dict d = {0};
    put(&d, "a", 10);
    put(&d, "a", 20);
    put(&d, "b", "hello world");
    assert(get(&d, "a", int) == 20);
    assert(strcmp("hello world", get(&d, "b", char*)) == 0);
    printf(GREEN"[dict] simple insert PASSED.\n"DEFAULT);
}

int main() {
    test_array_put_get_str();
    test_dict_put_item();

    return 0;
}
