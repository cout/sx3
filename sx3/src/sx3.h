// File: sx3.h
// Author: Marc Bryant
// 
// Structures and defines common to all sx3 project files.
// NOTE: There should be NO global variables in this file!

#ifndef SX3_H
#define SX3_H

#include "sx3_errors.h"

#ifdef __cplusplus
extern "C" {
#endif


// ===========================================================================
// Global macros
// ===========================================================================

// At this point, these are still just dummy numbers.  In the future, we
// will probably be defining these values in a seperate file, so that the
// makefile can automatically update the SX3_BUILD_VERSION

// FIX ME!! These should be static const variables
#define SX3_MAJOR_VERSION 1
#define SX3_MINOR_VERSION 1
#define SX3_BUILD_VERSION 23


// ===========================================================================
// Global structures
// ===========================================================================

struct Point {
    float x;
    float y;
    float z;
};

union VPoint {
    struct Point p;
    float v[4];
};

struct IPoint {
    int x;
    int y;
    int z;
};

struct Color {
    float r;
    float g;
    float b;
    float a;
};

#ifdef __cplusplus
}
#endif
#endif
