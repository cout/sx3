#ifndef CTL__C_ALGO_H
#define CTL__C_ALGO_H

// There are a number of algorithms which are useful operations on CTL
// structures.  Generally speaking, each of these algorithms takes a 
// container type, a data type,  a begin iterator, an end iterator, and
// one or more parameters.

// The FOR_EACH operation performs an operation on every element of a
// data structure.
#define FOR_EACH(ctype, dtype, begin, end, op) \
  do { \
    ctype ## _IT(dtype) it = begin; \
    for(; (ctype ## _IT_LT(it, end)); ctype ## _IT_INC(it)) { \
      op(it); \
    } \
  } while(0)

// The COPY operation copies data from one location to another
#define COPY(ctype, dtype, begin, end, out) \
  do { \
    ctype ## _IT(dtype) in_it = begin; \
    ctype ## _IT(dtype) out_it = out; \
    for(; (ctype ## _IT_LT(in_it, end)); \
          ctype ## _IT_INC(in_it), ctype ## _IT_INC(out_it)) { \
      ctype ## _IT_GET(out_it) = ctype ## _IT_GET(in_it); \
    } \
  } while(0)

#endif // CTL__C_ALGO_H
