#pragma once

#include <stdio.h>
#define INTERNAL_ARR_SIZE 100
#define put(d, key, e) _put(d,key,(void*)e)
#define get(d, key, type)    (type)_get(d,key)

struct dict {
    void* _arr[INTERNAL_ARR_SIZE];
};

void _put(struct dict* d, char* key, void* e);
void* _get(struct dict* d, char* key);

