// File: sx3_math.c
// Author: Marc Bryant
//
// Mathematical functions

#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "sx3_terrain.h"
#include "sx3_global.h"
#include "sx3_math.h"



// ===========================================================================`
// GLobal variables (constant)
// ===========================================================================`

const sx3_matrix_4x4 g_zero_matrix = 
    { {0,0,0,0},
      {0,0,0,0},
      {0,0,0,0},
      {0,0,0,0} };  

const sx3_matrix_4x4 g_identity_matrix =
    { {1,0,0,0},
      {0,1,0,0},
      {0,0,1,0},
      {0,0,0,1} };  



// ===========================================================================`
// Function definitions
// ===========================================================================`

// sx3_Matrix_Identity_4x4 copies the identity matrix to the input matrix.
void sx3_Matrix_Identity_4x4(sx3_matrix_4x4 m)
{
    memcpy (m, &g_identity_matrix, sizeof (sx3_matrix_4x4));
}


// sx3_Matrix_Zero_4x4 copies the zero matrix to the input matrix.
void
sx3_Matrix_Zero_4x4(sx3_matrix_4x4 m)
{
    memcpy (m, &g_zero_matrix, sizeof (sx3_matrix_4x4));
}


// sx3_Matrix_Multiply_4x4_4x4
//   Multiply 2 4x4 matrixes and store the result in a third matrix.
//   This function makes the assumption that the input matrixes are a
//   certain brand of sparse matrixes.  _NEVER_ use this function with
//   anything but Translation or Rotation matrixes.
void sx3_Matrix_Multiply_4x4_4x4 (
    sx3_matrix_4x4 m1,
    sx3_matrix_4x4 m2,
    sx3_matrix_4x4 result
    )
{
    // First row
    result[0][0]=m1[0][0]*m2[0][0]+m1[0][1]*m2[1][0]+m1[0][2]*m2[2][0];
    result[0][1]=m1[0][0]*m2[0][1]+m1[0][1]*m1[1][1]+m1[0][2]*m2[2][1];
    result[0][2]=m1[0][0]*m2[0][2]+m1[0][1]*m2[1][2]+m1[0][2]*m2[2][2];
    result[0][3]=0;

    // Second row
    result[1][0]=m1[1][0]*m2[0][0]+m1[1][1]*m2[1][0]+m1[1][2]*m2[2][0];
    result[1][1]=m1[1][0]*m2[0][1]+m1[1][1]*m1[1][1]+m1[1][2]*m2[2][1];
    result[1][2]=m1[1][0]*m2[0][2]+m1[1][1]*m2[1][2]+m1[1][2]*m2[2][2];
    result[1][3]=0;

    // Third row
    result[2][0]=m1[2][0]*m2[0][0]+m1[2][1]*m2[1][0]+m1[2][2]*m2[2][0];
    result[2][1]=m1[2][0]*m2[0][1]+m1[2][1]*m1[1][1]+m1[2][2]*m2[2][1];
    result[2][2]=m1[2][0]*m2[0][2]+m1[2][1]*m2[1][2]+m1[2][2]*m2[2][2];
    result[2][3]=0;

    // Fourth row
    result[3][0]=m1[3][0]*m2[0][0]+m1[3][1]*m2[1][0]+m1[3][2]*m2[2][0]+m2[3][0];
    result[3][1]=m1[3][0]*m2[0][1]+m1[3][1]*m1[1][1]+m1[3][2]*m2[2][1]+m2[3][1];
    result[3][2]=m1[3][0]*m2[0][2]+m1[3][1]*m2[1][2]+m1[3][2]*m2[2][2]+m2[3][2];
    result[3][3]=1;
}

// sx3_Matrix_Copy_4x4 copies one matrix to another.
void sx3_Matrix_Copy_4x4(sx3_matrix_4x4 destination, sx3_matrix_4x4 source)
{
    memcpy (destination, source, sizeof (sx3_matrix_4x4));
}

