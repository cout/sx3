// File: sx3_graphics.h
// Author: Marc Bryant
//
// Graphics module header file

#ifndef SX3_GRAPHICS_H
#define SX3_GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sx3.h"


// ===========================================================================
// Data types
// ===========================================================================
struct Object_Model {
    int dummy_value;
};


// ===========================================================================
// Function protoypes
// ===========================================================================

SX3_ERROR_CODE sx3_update_screen ( 
    struct Point eye_point,
    struct Point view_point,
    struct Point view_dir,
    struct Point up_vector,
    float dt);
void sx3_gl_settings();
void resize(int width, int height);
void sx3_init_graphics();
void sx3_close_graphics();

#ifdef __cplusplus
}
#endif
#endif

