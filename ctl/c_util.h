#ifndef CTL__C_UTIL_H
#define CTL__C_UTIL_H

#include <stdlib.h>

#if defined(_XOPEN_SOURCE) || defined(_BSD_SOURCE) || defined(_GNU_SOURCE)
#define CTL_TMP_ALLOC(x) alloca(x)
#define CTL_TMP_FREE(x) do {} while(0)
#else
#define CTL_TMP_ALLOC(x) malloc(x)
#define CTL_TMP_FREE(x) free(x)
#endif

#endif
