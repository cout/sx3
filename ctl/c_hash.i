#ifndef CTL__C_HASH_I
#define CTL__C_HASH_I

#include <stdlib.h>
#include "c_algo.h"
#include "c_itor.h"
#include "c_types.h"
#include "c_util.h"

typedef HASH_FUNCTION(void, CTL__Hash_Function);
typedef HASH(char, char) CTL__Hash;

inline void ctl__hash_init(
        CTL__Hash *h,
        size_t element_size,
        size_t hash_size,
        CTL__Hash_Function f) {

    size_t j;

    h->hash = (void *)f;
    h->data = malloc(hash_size * sizeof(HASH_BUCKET(char, char)));
    h->size = hash_size;
    h->element_size = element_size;
    
    for(j = 0; j < h->size; ++j) {
        h->data[j].count = 0;
        h->data[j].max = 1;
        h->data[j].data = malloc(element_size);
    }
}

#define CTL__HASH_POST_INIT(h, ksize, kstart, vsize, vstart) \
    do { \
        h.key_size = ksize; \
        h.key_start = kstart; \
        h.value_size = vsize; \
        h.value_start = vstart; \
    } while(0)

inline void ctl__hash_free(CTL__Hash *h) {
    size_t j;
    for(j = 0; j < h->size; ++j) {
        free(h->data[j].data);
    }
    free(h->data);
}

void ctl__hash_insert(CTL__Hash *h, void *key, void *value)
{
    size_t hash = h->hash(key) % h->size;
    if(h->data[hash].count == h->data[hash].max) {
        h->data[hash].max *= 2;
        h->data[hash].data = realloc(
            h->data[hash].data,
            h->data[hash].max * h->element_size);
    }
    COPY(
        ARRAY,
        char,
        (char *)key,
        (char *)key + h->key_size,
        (char *)h->data[hash].data +
            h->element_size * h->data[hash].count +
            h->key_start);
    COPY(
        ARRAY,
        char,
        (char *)value,
        (char *)value + h->value_size,
        (char *)h->data[hash].data +
            h->element_size * h->data[hash].count +
            h->value_start);
    h->data[hash].count++;
}

// Note that "result" is a HASH_ELEMENT and not a value.
inline CTL_BOOL ctl__hash_find(
        CTL__Hash *h,
        void *key,
        CTL_COMPARISON_FUNCTION(void *, compare),
        void *result) {

    size_t hash = h->hash(key) % h->size;
    HASH_BUCKET(char, char) *bucket = (void *)&h->data[hash];
    char *it = (char *)bucket->data;
    char *end = it + h->element_size * bucket->count;
    HASH_ELEMENT(char, char) *elem;

    while(it < end) {
        elem = (void *)it;
        if(compare(&elem->key, key) == 0) {
            COPY(
                ARRAY,                                  // container type
                char,                                   // data type
                (char *)elem,                           // input begin
                (char *)elem + h->element_size,         // input end
                (char *)result);                        // output begin
            return TRUE;
        }
        it += h->element_size;
    }

    return FALSE;
}

#endif // CTL__C_HASH_I
