// File: sx3_math.h
// Author: Marc Bryant
//
// Mathematical structures, defines, and prototypes.

#ifndef SX3_MATH_H
#define SX3_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <matrix.h>

#ifdef __cplusplus
#define INLINE inline
#else
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199401
#define INLINE static inline
#else
#define INLINE static __attribute__((unused))
#endif
#endif


// ===========================================================================
// Global macros
// ===========================================================================

#ifndef M_PI
#define M_PI 3.1415926535793238462643383
#endif

#define D2R(x) ((x)*2*M_PI/360.0)
#define R2D(x) ((x)*360.0/2/M_PI)

#define RANGE_CHECK(a,low,hi) \
    do { \
        if(a<(low)) a = (low); \
        if(a>(hi)) a = (hi); \
    } while(0)

#define RANGE_CHECK_WRAP(a,low,hi) \
    do { \
        while(a<(low)) a+=((hi)-(low)); \
        while(a>(hi)) a-=((hi)-(low)); \
    } while(0)


// ===========================================================================
// Data types
// ===========================================================================

typedef float sx3_matrix_4x4[4][4];


// ===========================================================================
// Global variables
// ===========================================================================

extern const sx3_matrix_4x4 g_zero_matrix;

extern const sx3_matrix_4x4 g_identity_matrix;

// ===========================================================================
// Fast math
// ===========================================================================

static int T1[32]= {
    0,  1024,   3062,   5746,   9193,   13348,  18162,  23592,
    29598,  36145,  43202,  50740,  58733,  67158,  75992,  85215,
    83599,  71378,  60428,  50647,  41945,  34246,  27478,  21581,
    16499,  12183,  8588,   5674,   3403,   1742,   661,    130,};

// This sqrt function is the algorithm described in glibc.  So we need to
// reference at least one other source here, for copyright purposes (FIX ME!!).
INLINE float fast_sqrt(float x) {
    float y;
    int k, x0, y0;

    x0 = *(int*)(&x);
    k = (x0 >> 1) + 0x1ff80000;
    y0 = k - T1[31 & (k>>15)];
    y = *(float*)(&y0);
    y = (y + x/y)/2;
    y = (y + x/y)/2;
    // y = y-(y-x/y)/2;
    return y;
}

// ===========================================================================
// Function declarations
// ===========================================================================

void 
sx3_Matrix_Identity_4x4 (
    sx3_matrix_4x4 m
    );

void
sx3_Matrix_Zero_4x4 (
    sx3_matrix_4x4 m
    );

void 
sx3_Matrix_Multiply_4x4_4x4 (
    sx3_matrix_4x4 m1,
    sx3_matrix_4x4 m2,
    sx3_matrix_4x4 result
    );

void
sx3_Matrix_Copy_4x4 (
    sx3_matrix_4x4 destination,
    sx3_matrix_4x4 source
    );

// FIX ME!! These functions can still be really slow, even with inlining
// (due to passing structs by value)
INLINE struct Point sx3_normalize(struct Point v)
{
    struct Point p;
    float mag = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if(mag == 0.0) // FIX ME!! Perhaps this should be a call to assert()?
    {
        fprintf(stderr, "ERROR: zero length vector!\n");
        exit(1);
    }
    p.x = v.x / mag;
    p.y = v.y / mag;
    p.z = v.z / mag;
    return p;
}

// sx3_norm_cross_prod calculates the sx3_normalized cross product of 2 vectors.
INLINE struct Point sx3_norm_cross_prod(struct Point v1, struct Point v2)
{
    struct Point out;

    out.x = v1.y*v2.z - v1.z*v2.y;
    out.y = v1.z*v2.x - v1.x*v2.z;
    out.z = v1.x*v2.y - v1.y*v2.x;
    return sx3_normalize(out);
}


#undef INLINE

#ifdef __cplusplus
}
#endif
#endif
