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
// Global macros
// ===========================================================================

// FIX ME!! This should be a static const variable.
#define MAX_SEARCH_STRING_LENGTH 80


// ===========================================================================
// Global variables 
// ===========================================================================

// Note: use the global variable access functions (see below) when speed is
// not a crucial issue

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

// View ----------------------------------------------------------------------
float               g_fov                       = (float)M_PI/4;
int                 g_sat_view_on               = 0;
float               g_sat_altitude              = 1000.0;
int                 g_view_radius               = 512;
float               g_zoom_increment            = 3.0F;
float               g_view_altitude             = 3.0F;
int                 g_view_gravity              = 1;


// Terrain rendering options -------------------------------------------------
int                 g_terrain_wire              = 0;      
int                 g_terrain_lights            = 1;
int                 g_terrain_detail_levels     = 6;
int                 g_terrain_detail_cutoff     = 4; 
int                 g_terrain_detail_skip       = 2;
int                 g_terrain_detail_alg        = 0;

// Misc. display options -----------------------------------------------------
int                 g_fog_type                  = 4;
int                 g_display_frame_rate        = 1;

// Console options -----------------------------------------------------------
int                 g_console_lines_per_page    = 10;
int                 g_console_active            = 0;  
struct IPoint       g_console_pos               = { 5, 5, 0 };
struct IPoint       g_console_size              = { 300, 180, 0 };

// Size of screen ------------------------------------------------------------
struct IPoint       g_window_size               = { 640, 480, 0 };

// Frame rate variables ------------------------------------------------------
float               g_frame_time                = 0.0F;      
int                 g_total_frames              = 0;

// Key mappings --------------------------------------------------------------
char                g_keymap_activate_console   = '`';


// ===========================================================================
// Global variable access database (types, enums, vars)
// ===========================================================================

// Data types we support
enum sx3_global_data_types {
    SX3_GLOBAL_STRING,
    SX3_GLOBAL_INT,    // int
    SX3_GLOBAL_BOOL,   // int as well -- FIX ME!! Perhaps this should be char?
    SX3_GLOBAL_FLOAT,  // float
    SX3_GLOBAL_CHAR,   // char
    SX3_GLOBAL_LAST
};

// generic global variable structure
struct sx3_global_variable {
    char                         *name;
    enum sx3_global_data_types   data_type;
    void                         *data;
    int                          max_size;       // Only for strings
    int                          read_only;
};

// The global variable database itself:
// In order to be able to access global variables from the Global Var Access
// Functions, we need to define those variables in the following list.  The
// format is:
//   { "variable_name",
//     variable_type,
//     ptr_to_variable,
//     length (strings only),
//     read_only
//   }
//
// Obviously, the global variables are hard-coded
// Into sx3_global.c.  I chose to hard-code the variables instead of having all
// the variables defined in a file.  This allows other modules to access the
// variables directly (instead of through a pointer to that variable) if they
// choose to.  I realize that this will not save a whole lot of time, but it is
// my hope that having one less level of indirection will help a little in some
// of the time  critical graphics routines.
//
// But most modules and functions will want to access the global variables 
// through the access functions for the global variable database, thereby
// avoiding direct access to the global variables.

const static struct sx3_global_variable sx3_global_varlist[] = {
    {"major_version",           SX3_GLOBAL_INT,     &g_major_version,               0, 1},
    {"minor_version",           SX3_GLOBAL_INT,     &g_minor_version,               0, 1},
    {"build_version",           SX3_GLOBAL_INT,     &g_build_version,               0, 1},
    {"terrain_wire",            SX3_GLOBAL_BOOL,    &g_terrain_wire,                0, 0},
    {"terrain_lights",          SX3_GLOBAL_BOOL,    &g_terrain_lights,              0, 0},
    {"terrain_detail_levels",   SX3_GLOBAL_INT,     &g_terrain_detail_levels,       0, 0},
    {"terrain_detail_cutoff",   SX3_GLOBAL_INT,     &g_terrain_detail_cutoff,       0, 0},
    {"terrain_detail_skip",     SX3_GLOBAL_INT,     &g_terrain_detail_skip,         0, 0},
    {"terrain_detail_alg",      SX3_GLOBAL_INT,     &g_terrain_detail_alg,          0, 0},
    {"fog_type",                SX3_GLOBAL_INT,     &g_fog_type,                    0, 0},
    {"display_frame_rate",      SX3_GLOBAL_BOOL,    &g_display_frame_rate,          0, 0},
    {"fov",                     SX3_GLOBAL_FLOAT,   &g_fov,                         0, 0},
    {"sat_view",                SX3_GLOBAL_BOOL,    &g_sat_view_on,                 0, 0},
    {"sat_altitude",            SX3_GLOBAL_FLOAT,   &g_sat_altitude,                0, 0},
    {"console_lines_per_page",  SX3_GLOBAL_INT,     &g_console_lines_per_page,      0, 0},
    {"console_active",          SX3_GLOBAL_BOOL,    &g_console_active,              0, 1},
    {"console_pos_x",           SX3_GLOBAL_INT,     &g_console_pos.x,               0, 0},
    {"console_pos_y",           SX3_GLOBAL_INT,     &g_console_pos.y,               0, 0},
    {"console_size_x",          SX3_GLOBAL_INT,     &g_console_size.x,              0, 0},
    {"console_size_y",          SX3_GLOBAL_INT,     &g_console_size.y,              0, 0},
    {"window_size_x",           SX3_GLOBAL_INT,     &g_window_size.x,               0, 1},
    {"window_size_y",           SX3_GLOBAL_INT,     &g_window_size.y,               0, 1},
    {"terrain_size_x",          SX3_GLOBAL_INT,     &g_terrain_size.x,              0, 1},
    {"terrain_size_y",          SX3_GLOBAL_INT,     &g_terrain_size.y,              0, 1},
    {"view_radius",             SX3_GLOBAL_INT,     &g_view_radius,                 0, 0},
    {"zoom_increment",          SX3_GLOBAL_FLOAT,   &g_zoom_increment,              0, 0},
    {"view_height",             SX3_GLOBAL_FLOAT,   &g_view_altitude,               0, 0},
    {"view_gravity",            SX3_GLOBAL_BOOL,    &g_view_gravity,                0, 0},
    {"frame_time",              SX3_GLOBAL_FLOAT,   &g_frame_time,                  0, 0},
    {"total_frames",            SX3_GLOBAL_INT,     &g_total_frames,                0, 0},
    {"km_activate_console",     SX3_GLOBAL_CHAR,    &g_keymap_activate_console,     0, 0},
    {"*end*",                   SX3_GLOBAL_INT,     0,                              0, 1}
};
                                                                                    

// ===========================================================================
// Global variable access functions
// ===========================================================================

// Note: use these functions to access the global variables when speed is
// not a crucial issue (ie: most of the time).

// sx3_init_global_vars
//
// does nothing at this point.
SX3_ERROR_CODE sx3_init_global_vars()
{
    // ok - so stuff has changed here.
    // maybe we should use this function to read in the defaults.
    return SX3_ERROR_SUCCESS;
}


// find_sx3_global_variable 
//
// finds the desired variable and returns the index for that var.
//
// INPUT:  var - the name of the desired variable
// RETURN: <0 - variable not found
//         >=0 - index of variable
int find_sx3_global_variable(const char *var)
{
    int i = -1;

    while (sx3_global_varlist[++i].data)
        if (0 == strcasecmp(var, sx3_global_varlist[i].name))
            return i;

    return -1;
}


// sx3_print_global_value 
//
// Outputs the desired value to buffer (in ASCII format).
//
// INPUT:  var - The name of the variable to print
//         buffer - pointer to the buffer that will accept the value
//         length - size in bytes of buffer
// OUTPUT: length - number of bytes copied to buffer (excluding terminating NULL)
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_BUFFER_TOO_SMALL 
SX3_ERROR_CODE sx3_print_global_value(
    const char *var, 
    char *buffer, 
    int *length)
{
    int var_index;
    char temp_string[32];

    // Does the global variable exist?
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // This function behaves differently depending on the variable's data type
    switch (sx3_global_varlist[var_index].data_type)
    {
        case SX3_GLOBAL_STRING:
            if((int)(strlen(sx3_global_varlist[var_index].data) + 1) > *length)
                return SX3_ERROR_BUFFER_TOO_SMALL;
            strcpy(buffer, sx3_global_varlist[var_index].data);
            *length = strlen (buffer);
            break;

        case SX3_GLOBAL_INT:
            // don't forget the terminating null character!
            if((sprintf (temp_string,"%d",*(int*)
                    (sx3_global_varlist[var_index].data)) + 1) > (*length))
                return SX3_ERROR_BUFFER_TOO_SMALL;
            strcpy(buffer, temp_string);
            *length = strlen (buffer);
            break;

        case SX3_GLOBAL_BOOL:  // This is actually an int -- FIX ME!! (char?)
            // don't forget the terminating null character!
            if((int)(strlen ("false") + 1) > *length)
                return SX3_ERROR_BUFFER_TOO_SMALL;
            if(*(int*)(sx3_global_varlist[var_index].data))
                strcpy(buffer, "True");
            else
                strcpy(buffer, "False");
            break;

        case SX3_GLOBAL_FLOAT:
            // don't forget the terminating null character!
            if((sprintf (temp_string,"%f",*(float*)
                    (sx3_global_varlist[var_index].data)) + 1) > *length)
                return SX3_ERROR_BUFFER_TOO_SMALL;
            strcpy (buffer, temp_string);
            *length = strlen (buffer);
            break;

        case SX3_GLOBAL_CHAR:
            if(*length < 1)
                return SX3_ERROR_BUFFER_TOO_SMALL;
            *buffer = *(char*)(sx3_global_varlist[var_index].data);
            *(buffer+1) = '\0';
            *length = 1;
            break;

        default:
            break;
            // We should never reach this point!!!!
    }

    return SX3_ERROR_SUCCESS;
}


// sx3_set_global_int
// 
// Sets the desired global variable to the value specified 
//
// INPUT:  var - The name of the variable to set
//         value - the new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
//         SX3_ERROR_G_VAR_READ_ONLY
SX3_ERROR_CODE sx3_set_global_int(const char *var, const int value)
{
    int var_index;

    // Does the global variable exist?
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type?
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_INT)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the variable read only ?
    if (sx3_global_varlist[var_index].read_only)
        return SX3_ERROR_G_VAR_READ_ONLY;

    *(int*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
}


// sx3_set_global_float
// 
// Sets the desired global variable to the value specified 
//
// INPUT:  var - The name of the variable to set
//         value - the new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
//         SX3_ERROR_G_VAR_READ_ONLY
//
SX3_ERROR_CODE sx3_set_global_float(const char *var, const float value)
{
    int var_index;

    // Does the global variable exist?
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type?
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_FLOAT)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the variable read only ? 
    if (sx3_global_varlist[var_index].read_only)
        return SX3_ERROR_G_VAR_READ_ONLY;

    *(float*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
} 


// sx3_set_global_string
// 
// Sets the desired global variable to the value specified 
//
// INPUT:  var - The name of the variable to set
//         value - pointer to the new string buffer (NULL terminated)
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
//         SX3_ERROR_G_VAR_STRING_TOO_SHORT
//         SX3_ERROR_G_VAR_READ_ONLY
SX3_ERROR_CODE sx3_set_global_string(const char *var, const char *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_STRING)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the global variable string long enough?
    // (remember the terminating NULL) 
    if ( (int)(strlen(value) + 1) > 
         sx3_global_varlist[var_index].max_size )
         return SX3_ERROR_G_VAR_STRING_TOO_SHORT;

    // Is the variable read only ? 
    if (sx3_global_varlist[var_index].read_only)
        return SX3_ERROR_G_VAR_READ_ONLY;

    strcpy (sx3_global_varlist[var_index].data, value);

    return SX3_ERROR_SUCCESS;
}


// sx3_set_global_bool
// 
// Sets the desired global variable to the value specified 
//
// INPUT:  var - The name of the variable to set
//         value - The new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_set_global_bool(const char *var, const int value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_BOOL)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the variable read only ? 
    if (sx3_global_varlist[var_index].read_only)
        return SX3_ERROR_G_VAR_READ_ONLY;

    *(int*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
}


// sx3_set_global_char
// 
// Sets the desired global variable to the value specified 
//
// INPUT:  var - The name of the variable to set
//         value - the new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
//         SX3_ERROR_G_VAR_READ_ONLY
SX3_ERROR_CODE
sx3_set_global_char (
    const char *var, 
    const char value
    )
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_CHAR)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the variable read only ? 
    if (sx3_global_varlist[var_index].read_only)
        return SX3_ERROR_G_VAR_READ_ONLY;

    *(char*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
}

   
// sx3_get_global_int
// 
// Returns the value of the specified global variable
//
// INPUT:  var - The name of the variable to retrieve
// OUTPUT: value - The value of the desired variable
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_get_global_int(const char *var, int *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_INT)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *value = *(int*)(sx3_global_varlist[var_index].data);

    return SX3_ERROR_SUCCESS;
}


// sx3_get_global_float
// 
// Returns the value of the specified global variable
//
// INPUT:  var - The name of the variable to retrieve
// OUTPUT: value - The value of the desired variable
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_get_global_float(const char *var, float *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_FLOAT)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *value = *(float*)(sx3_global_varlist[var_index].data);

    return SX3_ERROR_SUCCESS;
}


// sx3_get_global_bool
// 
// Returns the value of the specified global variable
//
// INPUT:  var - The name of the variable to retrieve
// OUTPUT: value - The value of the desired variable
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_get_global_bool(const char *var, int *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_BOOL)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *value = *(int*)(sx3_global_varlist[var_index].data);

    return SX3_ERROR_SUCCESS;
}


// sx3_get_global_string
// 
// Returns the value of the specified global variable
//
// INPUT:  var - The name of the variable to retrieve
//         value - Ptr to buffer where we want the global string copied
//         length - Size (in bytes) of the 'value' buffer
// OUTPUT: length - The number of bytes copied to 'value' buffer (excluding the
//                  terminating NULL)
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
//         SX3_ERROR_BUFFER_TOO_SMALL
SX3_ERROR_CODE sx3_get_global_string (
    const char *var, 
    char *value,
    int *length)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_STRING)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the 'value' buffer long enough?  (Don't forget about terminating NULL) 
    if ( (int)(strlen(sx3_global_varlist[var_index].data) + 1) > *length )
            return SX3_ERROR_BUFFER_TOO_SMALL;

    strcpy (value, sx3_global_varlist[var_index].data);
    *length = strlen (value);

    return SX3_ERROR_SUCCESS;
}


// sx3_get_global_char
// 
// Returns the value of the specified global variable
//
// INPUT:  var - The name of the variable to retrieve
// OUTPUT: value - The value of the desired variable
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_get_global_char(const char *var, char *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_CHAR)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *value = *(char*)(sx3_global_varlist[var_index].data);

    return SX3_ERROR_SUCCESS;
}

   
// sx3_set_global_value
// 
// Sets a variable to the desired value.  
//
// INPUT:  var - The name of the variable to retrieve
//         value - Ptr to string containing new value
// OUTPUT: 
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_STRING_TOO_SHORT
//         SX3_ERROR_G_VAR_READ_ONLY
//         SX3_ERROR_G_VAR_BAD_DATA_TYPE

SX3_ERROR_CODE
sx3_set_global_value (
    const char *var, 
    const char *value
    )
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Determine the data type 
    switch (sx3_global_varlist[var_index].data_type)
    {
        case SX3_GLOBAL_STRING:
            return sx3_set_global_string (var, value);
            break;
        case SX3_GLOBAL_INT:
            return sx3_set_global_int (var, atoi(value));
            break;
        case SX3_GLOBAL_BOOL:
            if ( 0==strcasecmp(value, "true") ||
                 0==strcasecmp(value, "on") )
                return sx3_set_global_bool (var, 1);
            if ( 0==strcasecmp(value, "false") ||
                 0==strcasecmp(value, "off") )
                return sx3_set_global_bool (var, 0);
            return sx3_set_global_bool (var, atoi(value));
            break;
        case SX3_GLOBAL_FLOAT:
            return sx3_set_global_float (var, (float)atof(value));
            break;
        case SX3_GLOBAL_CHAR:
            return sx3_set_global_char (var, *value);
            break;
        default:
            // We should never get here 
            return SX3_ERROR_G_VAR_BAD_DATA_TYPE;
            break;
    }
}


// sx3_force_set_global_value
// 
// Sets a variable to the desired value, regardless of read-only 
// status.
//
// INPUT:  var - The name of the variable to retrieve
//         value - Ptr to string containing new value
// OUTPUT: 
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_STRING_TOO_SHORT
SX3_ERROR_CODE sx3_force_set_global_value ( const char *var, const char *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Determine the data type 
    switch (sx3_global_varlist[var_index].data_type)
    {
        case SX3_GLOBAL_STRING:
            return sx3_force_set_global_string (var, value);
            break;
        case SX3_GLOBAL_INT:
            return sx3_force_set_global_int (var, atoi(value));
            break;
        case SX3_GLOBAL_BOOL:
            if ( 0==strcasecmp(value, "true") ||
                 0==strcasecmp(value, "on") )
                return sx3_force_set_global_bool (var, 1);
            if ( 0==strcasecmp(value, "false") ||
                 0==strcasecmp(value, "off") )
                return sx3_force_set_global_bool (var, 0);
            return sx3_force_set_global_bool (var, atoi(value));
            break;
        case SX3_GLOBAL_FLOAT:
            return sx3_force_set_global_float (var, (float)atof(value));
        default:
            // We should never get here 
            return SX3_ERROR_G_VAR_BAD_DATA_TYPE;
            break;
    }
}

   
// sx3_force_set_global_int
// 
// Sets the desired global variable to the value specified, regardless of read-only 
// status. 
//
// INPUT:  var - The name of the variable to set
//         value - the new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_force_set_global_int(const char *var, const int value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_INT)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *(int*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
} 


// sx3_force_set_global_float
// 
// Sets the desired global variable to the value specified, regardless of read-only 
// status.
//
// INPUT:  var - The name of the variable to set
//         value - the new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_force_set_global_float(const char *var, const float value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_FLOAT)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *(float*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
}


// sx3_force_set_global_string
// 
// Sets the desired global variable to the value specified, regardless of read-only 
// status.
//
// INPUT:  var - The name of the variable to set
//         value - pointer to the new string buffer (NULL terminated)
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
//         SX3_ERROR_G_VAR_STRING_TOO_SHORT
SX3_ERROR_CODE sx3_force_set_global_string(const char *var, const char *value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_STRING)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    // Is the global variable string long enough? (remember the terminating NULL) 
    if ( (int)(strlen(value) + 1) > 
         sx3_global_varlist[var_index].max_size )
         return SX3_ERROR_G_VAR_STRING_TOO_SHORT;

    strcpy (sx3_global_varlist[var_index].data, value);

    return SX3_ERROR_SUCCESS;
}


// sx3_force_set_global_bool
// 
// Sets the desired global variable to the value specified, regardless of read-only 
// status.
//
// INPUT:  var - The name of the variable to set
//         value - The new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_force_set_global_bool(const char *var, const int value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_BOOL)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *(int*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
}

   
// sx3_force_set_global_char
// 
// Sets the desired global variable to the value specified 
//
// INPUT:  var - The name of the variable to set
//         value - the new value for this variable
// OUTPUT: none
//
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_G_VAR_NOT_FOUND
//         SX3_ERROR_G_VAR_WRONG_TYPE
SX3_ERROR_CODE sx3_force_set_global_char(const char *var, const char value)
{
    int var_index;

    // Does the global variable exist? 
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return SX3_ERROR_G_VAR_NOT_FOUND;

    // Is the variable of the correct data type? 
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_CHAR)
        return SX3_ERROR_G_VAR_WRONG_TYPE;

    *(char*)(sx3_global_varlist[var_index].data) = value;

    return SX3_ERROR_SUCCESS;
}


// sx3_find_global_var
// 
// This function will search through the global variable database
// and return the names of any variables that match the search string.
// The function only compares the first n characters, where 
// n = strlen (search_string), so it can be used as a pseudo-
// command line completion search.  Only one var name is returned per
// call to sx3_find_global_var: subsequent calls with a NULL search
// string will return subsequent matching vars in the database.
//
// INPUT:  ss - Search string (var names are compared against this)
//              If (ss==NULL) then continue previous search.
// OUTPUT: var_name - The name of the next matching var
//              If (var_name==NULL) then no matches found.
//              CAUTION:
//              strlen (var_name) must be >= MAX_LINE_LENGTH
//
// RETURN: SX3_ERROR_SUCCESS
SX3_ERROR_CODE sx3_find_global_var(const char *ss, char *var_name)
{
    static char current_ss [MAX_SEARCH_STRING_LENGTH] = "";    
    static int var_index = 0;
    static int ss_length = 0;

    // Determine search string and length 
    if (ss!=NULL)
    {
        // New search string: initialize static vars 
        if ( (ss_length=strlen(ss)) >= MAX_SEARCH_STRING_LENGTH )
            ss_length = MAX_SEARCH_STRING_LENGTH-1;    
        strncpy (current_ss, ss, ss_length);
        current_ss [ss_length] = (char)0;
        var_index = 0;
    }

    // Return if there is not search defined 
    //if (0 == ss_length)
    {
        var_name [0] = (char)0;
        return SX3_ERROR_SUCCESS;
    }

    // Search for the var 
    while (sx3_global_varlist[var_index].data)
    {
        if ( (0 == strncasecmp(current_ss, 
                    sx3_global_varlist[var_index].name,
                    ss_length)) )
        {
            strcpy (var_name, sx3_global_varlist[var_index].name);
            var_index++;
            return SX3_ERROR_SUCCESS;  // Match found! 
        }
        var_index++;
    }

    // No matches found 
    var_name[0] = (char)0;
    return SX3_ERROR_SUCCESS;
}

SX3_ERROR_CODE sx3_read_config_file(const char *filename)
{
    INI_Context *ini = ini_new_context();
    const struct sx3_global_variable *var;
    const char *val;

    if(ini_load_config_file(ini, filename) != INI_OK)
    {
        ini_free_context(ini);
        return SX3_ERROR_CANNOT_OPEN_FILE;
    }

    for(var = sx3_global_varlist; var->data; ++var)
    {
        if((val = ini_get_value(ini, "Global", var->name)) != 0)
        {
            printf("Setting %s to %s\n", var->name, val);
            sx3_set_global_value(var->name, val);
        }
    }

    ini_free_context(ini);
    return SX3_ERROR_SUCCESS;
}
