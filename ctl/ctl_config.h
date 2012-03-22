#ifndef CTL__CTL_CONFIG_H
#define CTL__CTL_CONFIG_H

#if defined(_XOPEN_SOURCE) || defined(_BSD_SOURCE) || defined(_GNU_SOURCE)
#   define HAS_ALLOCA
#else
#   undef HAS_ALLOCA
#endif

#if defined(__GNUC__)
#   define HAS_TYPEOF
#else
#   undef HAS_TYPEOF
#endif

#endif

