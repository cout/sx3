#ifndef CTL__C_UTIL_H
#define CTL__C_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include "c_types.h"
#include "ctl_config.h"

#ifdef CTL_HAS_ALLOCA
#   define CTL_TMP_ALLOC(x) alloca(x)
#   define CTL_TMP_FREE(x) do {} while(0)
#else
#   define CTL_TMP_ALLOC(x) malloc(x)
#   define CTL_TMP_FREE(x) free(x)
#endif

#ifdef __cplusplus
#   define CTL_INLINE inline
#else
#   if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199401
#       define CTL_INLINE static inline
#   else
#       define CTL_INLINE static __attribute__((unused))
#   endif
#endif


#ifdef __cplusplus
#   define CTL_VOID_CAST(x) static_cast<void>(x)
#else
#   define CTL_VOID_CAST(x) (void)(x)
#endif

#ifdef DEBUG
#   define CTL_ASSERT_IO(x, io) \
        CTL_VOID_CAST( \
            (x ? x : (fprintf(io, "Assertion failed on line %d in %s: %s\n", \
            __LINE__, \
            __FILE__, \
            #x), x)))
#   define CTL_ASSERT_IS_TYPE_OF(x, type) { \
        type y = x; \
        CTL_UNUSED_ARG(y); \
        CTL_ASSERT_SAME_TYPE(x, y); \
    }
#   ifdef CTL_HAS_TYPEOF
#       define CTL_ASSERT_SAME_TYPE(x, y) { \
            CTL_BOOL sizes_match = (sizeof(x) == sizeof(y)); \
            CTL_ASSERT(sizes_match); \
            if(sizes_match) { \
                typeof(x) z;\
                z = y;\
            } \
        }
#   else
#       define CTL_ASSERT_SAME_TYPE(x, y) { \
            CTL_BOOL sizes_match = (sizeof(x) == sizeof(y)); \
            CTL_ASSERT(sizes_match); \
        }
#   endif
#else
#   define CTL_ASSERT_IO(x, io)
#   define CTL_ASSERT_IS_TYPE_OF(x, type) { \
        type y = x; \
        CTL_UNUSED_ARG(y); \
    }
#   define CTL_ASSERT_SAME_TYPE(x, y)
#endif

#define CTL_ASSERT(x) CTL_ASSERT_IO(x, stderr)

CTL_INLINE int ctl_null_function_call(void) { return 0; }

#define CTL_ASSERT_EXISTS(x) (x ? ctl_null_function_call() : 0)

#endif
