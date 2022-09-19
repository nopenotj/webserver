#ifndef ARRAY_H_
#define ARRAY_H_

#define array_push(arr, e) _array_push(arr, (void *)e)

struct array {
    void **_arr;
    int esize;
    int len;
    int max_len;
};
void _array_push(struct array* a, void* e);
void* array_get(struct array a, int i);
void array_free(struct array a);


typedef void(*unary_func)(void *,...);
void array_foreach(struct array*, unary_func);


#endif // ARRAY_H_
