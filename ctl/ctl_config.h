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

/* Shamelessly taken from ACE's config-all.h */
#if defined (ghs) || defined (__GNUC__) || defined (__hpux) || defined (__sgi) || defined (__DECCXX) || defined (__KCC) || defined (__rational__) || (__USLC__)
# define CTL_UNUSED_ARG(a) {if (&a) /* null */ ;}
#else /* ghs || __GNUC__ || ..... */
# define CTL_UNUSED_ARG(a) (a)
#endif /* ghs || __GNUC__ || ..... */

#endif

