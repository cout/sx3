/*
** sx3_global.h
**
**   Variables and prototypes needed to access the global variable database.
**
*/

#ifndef SX3_GLOBAL_H
#define SX3_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pglobal.h>

// ===========================================================================
// Global variables
// ===========================================================================

// View ----------------------------------------------------------------------
extern float               g_fov;
extern int                 g_sat_view_on;
extern float               g_sat_altitude;
extern int                 g_view_radius;
extern int                 g_view_gravity;

// Projectiles ---------------------------------------------------------------
extern int                 g_num_projectiles;
extern struct Projectile  *g_projectiles;

// Explosions ----------------------------------------------------------------
extern int                 g_num_explosions;
extern struct Explosion   *g_explosions;

// Tanks ---------------------------------------------------------------------
extern int                 g_num_tanks;
extern struct Tank        *g_tanks;
extern int                 g_current_tank;

// Frame rate variables ------------------------------------------------------
extern float               g_frame_time;

// Size of screen ------------------------------------------------------------
extern struct IPoint       g_window_size;

// NOTE: g_terrain_* are in sx3_terrain.h

#ifdef __cplusplus
}
#endif

#endif
