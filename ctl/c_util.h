#ifndef CTL__C_UTIL_H
#define CTL__C_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include "c_types.h"
#include "ctl_config.h"

#ifdef HAS_ALLOCA
#   define CTL_TMP_ALLOC(x) alloca(x)
#   define CTL_TMP_FREE(x) do {} while(0)
#else
#   define CTL_TMP_ALLOC(x) malloc(x)
#   define CTL_TMP_FREE(x) free(x)
#endif

#ifdef __cplusplus
#   define VOID_CAST(x) static_cast<void>(x)
#else
#   define VOID_CAST(x) (void)(x)
#endif

#ifdef DEBUG
#   define ASSERT_IO(x, io) \
        VOID_CAST( \
            (x ? x : (fprintf(io, "Assertion failed on line %d in %s: %s\n", \
            __LINE__, \
            __FILE__, \
            #x), x)))
#   define ASSERT_IS_TYPE_OF(x, type) { \
        type y = x; \
        UNUSED_ARG(y); \
        ASSERT_SAME_TYPE(x, y); \
    }
#   ifdef HAS_TYPEOF
#       define ASSERT_SAME_TYPE(x, y) { \
            CTL_BOOL sizes_match = (sizeof(x) == sizeof(y)); \
            ASSERT(sizes_match); \
            if(sizes_match) { \
                typeof(x) z;\
                z = y;\
            } \
        }
#   else
#       define ASSERT_SAME_TYPE(x, y) { \
            CTL_BOOL sizes_match = (sizeof(x) == sizeof(y)); \
            ASSERT(sizes_match); \
        }
#   endif
#else
#   define ASSERT_IO(x, io)
#   define ASSERT_IS_TYPE_OF(x, type) { \
        type y = x; \
        UNUSED_ARG(y); \
    }
#   define ASSERT_SAME_TYPE(x, y)
#endif

#define ASSERT(x) ASSERT_IO(x, stderr)

#define UNUSED_ARG(x) x=x

#endif
