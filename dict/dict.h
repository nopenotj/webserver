#pragma once

#include <stdio.h>
#include "../array/array.h"
#define INTERNAL_ARR_SIZE 100
#define dict_put(d, key, e) _dict_put(d,key,(void*)e)
#define dict_get(d, key, type)    (type)_dict_get(d,key)

struct dict {
    void* _arr[INTERNAL_ARR_SIZE];
    struct array keys;
};

void _dict_put(struct dict* d, char* key, void* e);
void* _dict_get(struct dict* d, char* key);

