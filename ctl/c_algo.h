#ifndef CTL__C_ALGO_H
#define CTL__C_ALGO_H

#include "c_itor.h"

// There are a number of algorithms which are useful operations on CTL
// structures.  Generally speaking, each of these algorithms takes a 
// container type, a data type,  a begin iterator, an end iterator, and
// one or more parameters.

// The FOR_EACH operation performs an operation on every element of a
// data structure.
#define FOR_EACH(ctype, dtype, begin, end, op) \
  do { \
    IT_TYPE(ctype, dtype) it = begin; \
    for(; (IT_LT(ctype, it, end)); IT_INC(ctype, it)) { \
      op; \
    } \
  } while(0)

// The COPY operation copies data from one location to another
#define COPY(ctype, dtype, begin, end, out) \
  do { \
    IT_TYPE(ctype, dtype) in_it = begin; \
    IT_TYPE(ctype, dtype) out_it = out; \
    for(; (IT_LT(ctype, in_it, end)); \
          IT_INC(ctype, in_it), IT_INC(ctype, out_it)) { \
      IT_GET(ctype, out_it) =IT_GET(ctype, in_it); \
    } \
  } while(0)

#endif // CTL__C_ALGO_H
