// File: sx3_global.c
// Author: Marc Bryant
//
// Global variables.  Most of these can be accesed through the sx3 global
// variable functions.  But for the sake of speed, most of these can also be
// accessed directly.  The global variable database only keeps pointers to 
// these variables.  
//
// Note: Please use the gloabl variable access functions to access the global
// variables whenever speed is not a critical issue.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ini.h>
#include "sx3_tanks.h"
#include "sx3_terrain.h"
#include "sx3_global.h"
#include "sx3_math.h"
#include "sx3_errors.h"


// ===========================================================================
// Global variables 
// ===========================================================================

// Version information variables ---------------------------------------------
int                 g_major_version             = SX3_MAJOR_VERSION;
int                 g_minor_version             = SX3_MINOR_VERSION;
int                 g_build_version             = SX3_BUILD_VERSION;

// Terrain global variables --------------------------------------------------
// These are also "declared" in sx3_terrain.h (instead of sx3_global.h)
struct IPoint       g_terrain_size;
float               g_terrain_vertex_height [MAX_MAP_X*MAX_MAP_Y];
struct Point        g_terrain_vertex_normal [MAX_MAP_X*MAX_MAP_Y];
struct Point        g_terrain_triag_normal  [MAX_MAP_X*MAX_MAP_Y*2];
long int            g_terrain_square_tile   [MAX_MAP_X*MAX_MAP_Y];

// Projectiles ---------------------------------------------------------------
int                 g_num_projectiles           = 0;
struct Projectile  *g_projectiles;

// Explosions ----------------------------------------------------------------
int                 g_num_explosions            = 0;
struct Explosion   *g_explosions;

// Tanks ---------------------------------------------------------------------
int                 g_num_tanks                 = 0;
struct Tank        *g_tanks;
int                 g_current_tank              = 0;

// Size of screen ------------------------------------------------------------
struct IPoint       g_window_size               = { 640, 480, 0 };

// Frame rate variables ------------------------------------------------------
float               g_frame_time                = 0.0F;      

