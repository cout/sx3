// File: sx3_misc.h
// Author: Paul Brannan
//
// Miscellaneous Sx3 utility functions and macros

#ifndef SX3_MISC_H
#define SX3_MISC_H

#include <stdlib.h>
#include <limits.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#define ONEXIT _onexit
#else
#define ONEXIT !atexit
#endif

#if defined(_WIN32) && !defined(__MINGW32__)
#define CLEANUP_TYPE int
#else
#define CLEANUP_TYPE void
#endif

#if defined(_WIN32) && !defined(__MINGW32__)
#define CLEANUP_RET(x) return x
#else
#define CLEANUP_RET(x)
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#endif
