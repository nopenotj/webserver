#include "array.h"
#include <stdlib.h>
#include <assert.h>

#include <stdio.h>


void _array_push(struct array *a, void* e) {
    if(a->len == a->max_len) {
        a->max_len = a->max_len ? a->max_len *2 : 1;
        a->_arr = realloc(a->_arr, a->max_len);
    }
    a->_arr[a->len++] = e;
}

void* array_get(struct array a, int i) {
    assert(a.len > i);
    return &a._arr[i];
}

void array_free(struct array a) {
    free(a._arr);
}
void array_foreach(struct array *a, unary_func f) {
    for (int i=0; i < a->len; i++) {
        f(array_get(*a, i));
    }
}
