#include "dict.h"
#include <string.h>


// TODO: improve string hash function
int hash(char* s) {
    int res = 0;
    for(;*s != '\0'; s++) res += *s;
    return res;
}
void _put(struct dict* d, char* key, void* e){
    // TODO: Add LL to deal with collisions
    d->_arr[hash(key) % INTERNAL_ARR_SIZE] = e;
}
void* _get(struct dict* d, char* key){
    return d->_arr[hash(key) % INTERNAL_ARR_SIZE];
}

