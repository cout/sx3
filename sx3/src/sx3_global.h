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
#include "sx3.h"

// ===========================================================================
// Global variables
// ===========================================================================

// Note: use the global variable access functions (see below) when speed is 
// not a crucial issue

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

// Terrain rendering options -------------------------------------------------
extern int                 g_terrain_wire;
extern int                 g_terrain_lights;
extern int                 g_terrain_detail_levels;
extern int                 g_terrain_detail_cutoff; 
extern int                 g_terrain_detail_skip;
extern int                 g_terrain_detail_alg;

// Misc. display options -----------------------------------------------------
extern int                 g_fog_type;
extern int                 g_display_frame_rate;

// Console options -----------------------------------------------------------
extern int                 g_console_active;  
extern struct IPoint       g_window_size;
extern float               g_zoom_increment;
extern float               g_view_altitude;

// Frame rate variables ------------------------------------------------------
float                      g_frame_time;
int                        g_total_frames;

// Key mappings --------------------------------------------------------------
extern char                g_keymap_activate_console;


// ===========================================================================
// Global variable access functions
// ===========================================================================

// Note: use these functions to access the global variables when speed is not
// a crucial issue.

SX3_ERROR_CODE sx3_init_global_vars        (void);
SX3_ERROR_CODE sx3_print_global_value      (const char *var, char *buffer, int *length);

SX3_ERROR_CODE sx3_set_global_value        (const char *var, const char *value);
SX3_ERROR_CODE sx3_set_global_int          (const char *var, const int   value);
SX3_ERROR_CODE sx3_set_global_string       (const char *var, const char *value);
SX3_ERROR_CODE sx3_set_global_bool         (const char *var, const int   value);
SX3_ERROR_CODE sx3_set_global_float        (const char *var, const float value);
SX3_ERROR_CODE sx3_set_global_char         (const char *var, const char  value);

SX3_ERROR_CODE sx3_force_set_global_value  (const char *var, const char *value);
SX3_ERROR_CODE sx3_force_set_global_int    (const char *var, const int   value);
SX3_ERROR_CODE sx3_force_set_global_string (const char *var, const char *value);
SX3_ERROR_CODE sx3_force_set_global_bool   (const char *var, const int   value);
SX3_ERROR_CODE sx3_force_set_global_float  (const char *var, const float value);
SX3_ERROR_CODE sx3_force_set_global_char   (const char *var, const char  value);

SX3_ERROR_CODE sx3_get_global_int          (const char *var, int   *value);
SX3_ERROR_CODE sx3_get_global_string       (const char *var, char  *value, int *length);
SX3_ERROR_CODE sx3_get_global_bool         (const char *var, int   *value);
SX3_ERROR_CODE sx3_get_global_float        (const char *var, float *value);
SX3_ERROR_CODE sx3_get_global_char         (const char *var, char  *value);
SX3_ERROR_CODE sx3_find_global_var         (const char *ss, char *var_name);


#ifdef __cplusplus
}
#endif
#endif
