#ifndef ARRAY_H_
#define ARRAY_H_

#define array_push(arr, e) _array_push(arr, (void *)e)
#define array_get(arr, i, type) *(type*)_array_get(arr, i)

// TODO: Add a way to iterate through arrays

struct array {
    void **_arr;
    int esize;
    int len;
    int max_len;
};
void _array_push(struct array* a, void* e);
void* _array_get(struct array a, int i);
void array_free(struct array a);


#endif // ARRAY_H_
