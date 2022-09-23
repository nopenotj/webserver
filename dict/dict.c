#include "dict.h"
#include <string.h>


// TODO: improve string hash function
int hash(char* s) {
    int res = 0;
    for(;*s != '\0'; s++) res += *s;
    return res;
}
void _dict_put(struct dict* d, char* key, void* e){
    // TODO: Add LL to deal with collisions
    array_push(&d->keys, key);
    d->_arr[hash(key) % INTERNAL_ARR_SIZE] = e;

}
void* _dict_get(struct dict* d, char* key){
    return d->_arr[hash(key) % INTERNAL_ARR_SIZE];
}

