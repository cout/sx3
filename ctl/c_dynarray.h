#ifndef CTL__C_DYNARRAY_H
#define CTL__C_DYNARRAY_H

// The DYNARRAY (dynamic array) type.  This is similar to a vector in the STL.
// We don't define quite so many operations, just init (the CTL version of
// a constructor), free (the CTL version of a destructor), and push_back (same
// as with the STL).

#define DYNARRAY(type) \
  struct { \
    size_t count; \
    size_t max; \
    size_t data_size; \
    type *data; \
  }

#define DYNARRAY_IT(type)         type*
#define DYNARRAY_CONST_IT(type)   const type*
#define DYNARRAY_BEGIN(da)        ((da).data)
#define DYNARRAY_END(da)          ((da).data + (da).count)
#define DYNARRAY_INIT(da)         ctl__da_init((CTL__Dynarray*)&da, \
                                    sizeof(da.data), 16)
#define DYNARRAY_INIT_SIZE(da, s) ctl__da_init((CTL__Dynarray*)&da, \
                                    sizeof(da.data), s)
#define DYNARRAY_FREE(da)         ctl__da_free((CTL__Dynarray*)&da)
#define DYNARRAY_PUSH_BACK(da, d) ctl__da_push_back((CTL__Dynarray*)&da, d)
#define DYNARRAY_PUSH_BACK_IMM(da, type, d) \
  do { \
    type data = d; \
    ctl__da_push_back((CTL__Dynarray*)&da, &data);\
  } while(0)

#define DYNARRAY_IT_LT(it1, it2)  ((it1) < (it2))
#define DYNARRAY_IT_EQ(it1, it2)  ((it1) == (it2))
#define DYNARRAY_IT_INC(it)       (++it)
#define DYNARRAY_IT_GET(it)       (*it)

#include "c_dynarray.i"

#endif // CTL__C_DYNARRAY_H
