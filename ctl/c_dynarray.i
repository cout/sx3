#ifndef CTL__C_DYNARRAY_I
#define CTL__C_DYNARRAY_I

#include <stdlib.h>
#include "c_algo.h"
#include "c_itor.h"

typedef DYNARRAY(void) CTL__Dynarray;

inline void ctl__da_init(CTL__Dynarray *a, size_t size, size_t initial_size) {
  a->count = 0;
  a->max = initial_size;
  a->data_size = size;
  a->data = malloc(size * initial_size);
}

inline void ctl__da_push_back(CTL__Dynarray *a, void *data) {
  if(a->count == a->max) {
    a->max *= 2;
    a->data = realloc(a->data, a->max * a->data_size);
  }
  COPY(
    ARRAY,                                           // container type
    char,                                            // data type
    (char *)data,                                    // input begin
    (char *)data + a->data_size,                     // input end
    (char *)a->data + a->data_size * a->count);      // output begin
    
  a->count++;
}

inline void ctl__da_free(CTL__Dynarray *a) {
  free(a->data);
}

#endif // CTL__C_DYNARRAY_I
