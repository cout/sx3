// File: sx3_utils.h
// Author: Paul Brannan
//
// Miscellaneous Sx3 utility functions and macros

#ifndef SX3_UTILS_H
#define SX3_UTILS_H

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


// STRUCTURES /////////////////////////////////////////////////////
struct Error_Message_Entry{
    int error_code;
    const char *error_message;
};


// PROTOTYPES /////////////////////////////////////////////////////
const char * error_lookup(
        const struct Error_Message_Entry * msgs,
        int error_code); 


#endif
