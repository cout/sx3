#ifndef CTL__C_UTIL_H
#define CTL__C_UTIL_H

#include <stdlib.h>
#include <stdio.h>

#if defined(_XOPEN_SOURCE) || defined(_BSD_SOURCE) || defined(_GNU_SOURCE)
#define CTL_TMP_ALLOC(x) alloca(x)
#define CTL_TMP_FREE(x) do {} while(0)
#else
#define CTL_TMP_ALLOC(x) malloc(x)
#define CTL_TMP_FREE(x) free(x)
#endif

#ifdef DEBUG
#define ASSERT(x) \
    if(!x) { \
        fprintf(stderr, "Assertion failed on line %d in %s: %s\n", \
            __LINE__, \
            __FILE__, \
            x); \
    }
#define ASSERT_IS_TYPE_OF(x, type) { \
    type y = x; \
    UNUSED_ARG(y); \
    ASSERT_SAME_TYPE(x, y); \
}
#define ASSERT_SAME_TYPE(x, y) { \
    ASSERT(sizeof(x) == sizeof(y)); \
}
#else
#define ASSERT(x)
#define ASSERT_IS_TYPE_OF(x, type) { \
    type y = x; \
    UNUSED_ARG(y); \
}
#define ASSERT_SAME_TYPE(x, y)
#endif

#define UNUSED_ARG(x) x=x

#endif
