//
// sx3_terrain.h
//
//   Terrain related structs
//


#ifndef SX3_TERRAIN_H
#define SX3_TERRAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sx3.h"


// ===========================================================================
// Global macros
// ===========================================================================

// FIX ME!! These should be static const variables

// Maximum x grid dimemsions for map: 
#define MAX_MAP_X                  1024    
// Maximum y grid dimensions for map: 
#define MAX_MAP_Y                  1024    
// Meters to grid side: 
#define METERS_PER_MAP_GRID        3.0F    
// Final width (z in gl) of terrain (meters): 
#define MAX_TERRAIN_WIDTH          (METERS_PER_MAP_GRID*MAX_MAP_X)
// Final depth (x in gl) of terrain (meters): 
#define MAX_TERRAIN_DEPTH          (METERS_PER_MAP_GRID*MAX_MAP_Y)
// Max height (y in gl) of terrain (meters): 
#define MAX_TERRAIN_HEIGHT         500.0F
// Maximum height allowed in input file 
#define MAX_FILE_TERRAIN_HEIGHT    0x1FFF    
// Maximum view diameter (in grid units) 
#define MAX_VIEW_DIAMETER          1024


// These macros convert from Map x and y coords to OpenGL x and z coords
// and vice versa.
//#define MAP_X_TO_GL_Z(macx)  ((float)((macx))/MAX_MAP_X * MAX_TERRAIN_WIDTH)
//#define MAP_Y_TO_GL_X(macy)  (((float)g_terrain_size.y/MAX_MAP_Y*MAX_TERRAIN_DEPTH) - (float)(macy)/MAX_MAP_Y*MAX_TERRAIN_DEPTH)
//#define GL_Z_TO_MAP_X(macz)  (//(int)((macz)/MAX_TERRAIN_WIDTH * MAX_MAP_X))
//#define GL_X_TO_MAP_Y(macx)  (//(int)( ((float)MAX_TERRAIN_DEPTH*((float)g_terrain_size.y/MAX_MAP_Y) - (macx)) / MAX_TERRAIN_DEPTH * MAX_MAP_Y))
#define MAP_X_TO_GL_Z(macx)  ((float)(macx)*METERS_PER_MAP_GRID)
#define MAP_Y_TO_GL_X(macy)  (((float)g_terrain_size.y*METERS_PER_MAP_GRID) - (float)(macy)*METERS_PER_MAP_GRID)
#define GL_Z_TO_MAP_X(macz)  ((macz)/METERS_PER_MAP_GRID)
#define GL_X_TO_MAP_Y(macx)  (( ((float)g_terrain_size.y) - (macx) / METERS_PER_MAP_GRID))


// ===========================================================================
// Data types
// ===========================================================================

enum Tile_Type {
    Snow,
    Blast,
    Pebbles,
    Rock,
    Dirt,
    Grass,
    Num_Tile_Types
};  // Tile_Type 


// ===========================================================================
// Global variables
// ===========================================================================

// Note: these variables actually reside in sx3_global.h.  They are merely
// declared here because they depend on some of the sx3_terrain.h #defines,
// and because they are terrain related. 
extern struct IPoint        g_terrain_size;
extern float                g_terrain_vertex_height [MAX_MAP_X*MAX_MAP_Y];
extern struct Point            g_terrain_vertex_normal [MAX_MAP_X*MAX_MAP_Y];
extern struct Point            g_terrain_triag_normal  [MAX_MAP_X*MAX_MAP_Y*2];
extern long int                g_terrain_square_tile   [MAX_MAP_X*MAX_MAP_Y];


// ===========================================================================
// Function declarations
// ===========================================================================

void sx3_draw_terrain_lights (void); 

SX3_ERROR_CODE sx3_load_terrain(
    char* terrainName, 
    float* buffer, 
    struct IPoint* terrainSize, 
    struct Point* normalBuffer, 
    struct Point* normalAvgBuffer);

SX3_ERROR_CODE sx3_draw_terrain( 
    struct Point current_pos, 
    struct Point current_view_dir,
    struct Point current_up_vector);

float sx3_interpolated_terrain_height(
    float x,
    float z,
    int res);

float sx3_find_terrain_height(float x, float y);

SX3_ERROR_CODE sx3_unload_terrain(void);

#ifdef __cplusplus
}
#endif
#endif
