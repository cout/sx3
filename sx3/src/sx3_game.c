// File: sx3_game.c
// Author: Paul Brannan
//
// This is the main game loop.  It uses SDL to grab keyboard and mouse events,
// and to perform the appropriate actions.  It will change the game mode
// between animate and normal mode as necessary.

#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <matrix.h>
#include <physics.h>
#include <pglobal.h>
#include <sx3_registry.h>
#include "sx3.h"
#include "sx3_math.h"
#include "sx3_game.h"
#include "sx3_global.h"
#include "sx3_terrain.h"
#include "sx3_tanks.h"
#include "sx3_files.h"
#include "sx3_state.h"
#include "sx3_console.h"
#include "sx3_engine.h"
#include "sx3_audio.h"
#include "sx3_gui.h"
#include <sx3_utils.h>


// ===========================================================================
// Global macros
// ===========================================================================

// FIX ME!! These should be static const variables
#define MOUSE_X_SENSITIVITY     0.5
#define MOUSE_Y_SENSITIVITY     0.5
#define WEAPON_ANGLE_DELTA      0.5
#define TURRET_ANGLE_DELTA      0.5
#define TANK_POWER_DELTA        1.0

#define MAX_EVENTS                16

// ===========================================================================
// Global variables
// ===========================================================================

int    current_window;

// viewing angle/distance data
double viewAngle = 71.9;    // 0 to 360
double viewPitch = 343.5;   // 0 to 360
double viewDistance = 16.1;
double viewStrafeH = 0.1;
double viewStrafeV = -3.7;
float  current_xz_angle;
float  current_xy_angle;

// FIX ME!!
// Okay, this is a hack.  I started changing something that I
// shouldn't have changed.  As a result, I have to change everything
// below this level if I want to continue going, or go through this
// entire file again if I want to undo what I've done.  So, this
// is the reason for what you see below.  We need to agree on what
// our representation for vectors will be.
union VPoint e, vp, vd, up;
#define eye_point e.v
#define view_point vp.v
#define view_dir vd.v
#define up_vector up.v

// Tank motion data
float tank_power_mod = 0.0;
float weapon_angle_mod = 0.0;
float turret_angle_mod = 0.0;

// Color of scene fog
GLfloat fog_color[] = {0.5F,0.5F,0.6F,1.0F};

// View variables -----------------------------------------------------------
float               g_fov                       = (float)M_PI/4;
int                 g_sat_view_on               = 0;
float               g_sat_altitude              = 1000.0;
int                 g_view_radius               = 512;
float               g_zoom_increment            = 3.0F;
float               g_view_altitude             = 3.0F;
int                 g_view_gravity              = 1;

// Misc. display options ---------------------------------------------------
int                 g_fog_type                  = 4;
int                 g_display_frame_rate        = 1;

// ===========================================================================
// Functions definitions
// ===========================================================================

// game_register_vars
//
// Registers the global varaibles specific to the game module
void sx3_game_register_vars (void)
{
    sx3_add_global_var ("view.fov",
                        SX3_GLOBAL_FLOAT,
                        0,
                        (void*)&g_fov,
                        0,
                        NULL);
    sx3_add_global_var ("view.satellite.enabled",
                        SX3_GLOBAL_BOOL,
                        0,
                        &g_sat_view_on,
                        0,
                        NULL);
    sx3_add_global_var ("view.satellite.altitude",
                        SX3_GLOBAL_FLOAT,
                        0,
                        &g_sat_altitude,
                        0,
                        NULL);
    sx3_add_global_var ("view.radius",
                        SX3_GLOBAL_INT,
                        0,
                        &g_view_radius,
                        0,
                        NULL);
    sx3_add_global_var ("view.speed",
                        SX3_GLOBAL_FLOAT,
                        0,
                        &g_zoom_increment,
                        0,
                        NULL);
    sx3_add_global_var ("view.altitude",
                        SX3_GLOBAL_FLOAT,
                        0,
                        &g_view_altitude,
                        0,
                        NULL);
    sx3_add_global_var ("view.gravity",
                        SX3_GLOBAL_BOOL,
                        0,
                        &g_view_gravity,
                        0,
                        NULL);
    sx3_add_global_var ("fog.type",
                        SX3_GLOBAL_INT,
                        0,
                        &g_fog_type,
                        0,
                        NULL);
    sx3_add_global_var ("frame_rate.enabled",
                        SX3_GLOBAL_BOOL,
                        0,
                        &g_display_frame_rate,
                        0,
                        NULL);
    return;
}  // game_register_vars


// init_game intializes our little demo game.
void init_game()
{
    // Iniitialize misc. subsystems
    sx3_init_audio();
    sx3_init_gui();
    sx3_init_graphics();
    sx3_console_init(SX3_DEFAULT_FONT_FILE);

    // Initialize terrain
    if (sx3_load_terrain(SX3_DEFAULT_TERRAIN,
                    g_terrain_vertex_height,
                    &g_terrain_size,
                    g_terrain_triag_normal,
                    g_terrain_vertex_normal))
    {
        fprintf(stderr, "Error loading terrain file %s!\n",
            SX3_DEFAULT_TERRAIN);
        exit (1);
    }

    // Initialize the physics engine
    // FIX ME!! These are not the right values, since we aren't doing
    // meter to GL conversions properly
    world_data.gravity = -0.98;
    world_data.air_density = 0.0;
    world_data.air_viscosity = 0.0;
    world_data.wind_x = 0.0;
    world_data.wind_z = 0.0;
    set_terrain_height_func(sx3_find_terrain_height);

    // Initialize the tanks
    // This MUST be done AFTER the terrain and physics initialization!
    sx3_init_tanks();
}

// close_game is called when the game is over
void close_game()
{
    sx3_close_audio();
    sx3_close_gui();
    sx3_close_graphics();
    sx3_unload_terrain();
    sx3_cleanup_tanks();
}

// init_gl does all the OpenGL intialization that needs to 
// be done only once, at the beginning.
static void init_gl()
{
    // initialize various gl states
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(fog_color[0],fog_color[1],fog_color[2],fog_color[3]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDrawBuffer(GL_BACK);

    // Enable face culling
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);

    // Reset misc. OpenGL parameters
    sx3_gl_settings();

    // specify modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    resize(g_window_size.x, g_window_size.y);
}

// reset_view resets the eyepoint and viewpoint to the origin.
static void reset_view()
{
    // Set view point 3 meters off the ground, in the
    eye_point[0] = g_terrain_size.y/2 * METERS_PER_MAP_GRID;
    eye_point[2] = g_terrain_size.x/2 * METERS_PER_MAP_GRID;
    eye_point[1] = sx3_find_terrain_height(eye_point[0],eye_point[2]) +
        g_view_altitude;

    view_dir[0] = 1.0F;
    view_dir[1] = 0.0F;
    view_dir[2] = 0.0F;

    vv_cpy(view_point, eye_point);
    vv_add(view_point, view_dir);

    up_vector[0] = 0.0F;
    up_vector[1] = 1.0F;
    up_vector[2] = 0.0F;

    current_xz_angle = 0.0F;
    current_xy_angle = 0.0F;

    return;
}

// This is where we update all the items in the scene
void sx3_game_update(float dt)
{
    if(get_game_mode() == SX3_GAME_ANIMATE && modify_scene(dt) == 0)
    {
        int i;

        for(i = 0; i < g_num_tanks; i++)
        {
            printf("Damage to tank %d: %f\n", i, g_tanks[i].s.temp_damage);
            g_tanks[i].s.energy -= g_tanks[i].s.temp_damage;
            g_tanks[i].s.temp_damage = 0.0;
            if(g_tanks[i].s.energy <= 0.0) {
                // FIX ME!! We should now remove the tank from the scene
                sx3_play_sound(SX3_AUDIO_BYEBYE);
                printf("Tank %d has been eliminated.\n", i);
            }
        }

        printf("Switching back to SX3_GAME mode\n");
        set_game_mode(SX3_GAME);
        g_current_tank++;
        g_current_tank %= g_num_tanks;
        printf("g_current_tank = %d\n", g_current_tank);
    }
}

void sx3_zoom(float increment)
{
    Vector v;
    vv_cpy(v, view_dir);
    vc_mul(v, increment);

    vv_add(eye_point, v);
    if(g_view_gravity > 0)
        eye_point[1] = sx3_find_terrain_height(eye_point[0],eye_point[2]) +
            g_view_altitude;
    vv_cpy(view_point, eye_point);
    vv_add(view_point, view_dir);
}

// sx3_game_animate animates the scene before it is drawn.
void sx3_game_animate(float dt)
    {
    Uint8 state = SDL_GetMouseState(NULL, NULL);

    if(state&(SDL_BUTTON_LMASK|SDL_BUTTON_RMASK))
    {
      if(state&SDL_BUTTON_LMASK&~SDL_BUTTON_RMASK)
          sx3_zoom(g_zoom_increment);
      else if(state&~SDL_BUTTON_LMASK&SDL_BUTTON_RMASK)
          sx3_zoom(-g_zoom_increment);
    }

    if(g_current_tank >= 0)
    {
        struct Tank *t = &g_tanks[g_current_tank];
        t->s.turret_angle += turret_angle_mod;
        t->s.weapon_angle += weapon_angle_mod;
        t->s.power += tank_power_mod;
    }
    
}

// sx3_game_key_hit processes all the keyboard commands.
void sx3_game_key_hit(SDLKey key, SDLMod mod, Uint8 state)
{
    if(state == SDL_RELEASED)
    {
        // A key was released.  Let's just reset everything and quit before
        // anyone sees us.
        tank_power_mod = 0.0;
        weapon_angle_mod = 0.0;
        turret_angle_mod = 0.0;
        return;
    }

    switch (key)
    {
        case ' ':
            if(get_game_mode() == SX3_GAME_ANIMATE) break;
            // Fire!
            // Note: we assume that the tank is not moving.
            init_scene();
            sx3_play_sound(SX3_AUDIO_SHOT);
            printf("Switching to SX3_GAME_ANIMATE mode\n");
            set_game_mode(SX3_GAME_ANIMATE);
            break;

        case 'l':
            // Toggle lighting
            // TODO: This should be fixed
            // g_terrain_lights = !g_terrain_lights;
            break;

        case 'f':
            // Toggle fog
            g_fog_type = (g_fog_type+1) % 5;
            sx3_gl_settings();
            break;

        case 'r':
            // Toggle frame rate display
            g_display_frame_rate = !g_display_frame_rate;
               break;

        case '.':
            // Toggle wireframe mode
            // TODO: This should be fixed
            // g_terrain_wire = !g_terrain_wire;
            sx3_gl_settings();
            break;

        case 'v':
            // Toggle satellite view
            g_sat_view_on = !g_sat_view_on;
            break;

        case '-':            tank_power_mod   = -TANK_POWER_DELTA;    break;
        case '_':            tank_power_mod   = -TANK_POWER_DELTA*10; break;
        case '=':            tank_power_mod   =  TANK_POWER_DELTA;    break;
        case '+':            tank_power_mod   =  TANK_POWER_DELTA*10; break;
        case SDLK_UP:        weapon_angle_mod =  WEAPON_ANGLE_DELTA;  break;
        case SDLK_DOWN:      weapon_angle_mod = -WEAPON_ANGLE_DELTA;  break;
        case SDLK_LEFT:      turret_angle_mod = -TURRET_ANGLE_DELTA;  break;
        case SDLK_RIGHT:     turret_angle_mod =  TURRET_ANGLE_DELTA;  break;

        case 'x':
        case 'q':
        case 27:
            // End the game
            set_game_mode(SX3_GAME_END);
            break;
        default:
            break;
    }
}

// sx3_game_mouse_motion processes the mouse input when the mouse is moved
void sx3_game_mouse_motion(Uint8 state, int x, int y, int xrel, int yrel)
{
    if ((state&SDL_BUTTON_LMASK&SDL_BUTTON_RMASK) || (state&SDL_BUTTON_MMASK))
    {
        // We have right and left mouse button drag or middle button drag!
        current_xy_angle -= ((float)(yrel)/g_window_size.y)*(float)M_PI;
        RANGE_CHECK_WRAP(current_xy_angle, -M_PI/2, M_PI/2);

        current_xz_angle += ((float)(xrel)/g_window_size.x)*(float)M_PI;
        RANGE_CHECK_WRAP(current_xz_angle, -M_PI*2, M_PI*2);

        view_dir[0] = (1.1F-(float)fabs(view_dir[1])) *
                      (float)cos(current_xz_angle);
        view_dir[1] = (float)sin(current_xy_angle);
        view_dir[2] = (1.1F-(float)fabs(view_dir[1])) *
                      (float)sin(current_xz_angle);

        vv_cpy(view_point, eye_point);
        vv_add(view_point, view_dir);
    }  // right and left mouse button drag
}
       
// Here lies the guts of the program.  This is the main loop that polls for
// SDL events, updates the scene, and refreshes the display.
void sx3_game()
{
    SDL_Event event;
    int count;
    Uint32 old_time = SDL_GetTicks(), time;
    float dt;

    set_game_mode(SX3_GAME);

    init_game();
    init_gl();
    reset_view();

    sx3_console_print ("*** Sx3 project (pre-alpha) ***");
    sx3_console_print ("");
    sx3_console_print ("Console commands available:");
    sx3_console_print ("  get <var name>");
    sx3_console_print ("  set <var name> <var value>");
    sx3_console_print ("  quit");
    sx3_console_print ("");

    while(get_game_mode() != SX3_GAME_END)
    {
        for(count = 0; count < MAX_EVENTS && SDL_PollEvent(&event); count++)
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    if(sx3_console_process_input(event.key.keysym.unicode&0x7F))
                    {
                        // Reset all the GL parameters if the user pressed
                        // return while in the console.
                        if (event.key.keysym.mod == SDLK_RETURN)
                            sx3_gl_settings();
                    }
                    else
                    {
                        // FIX ME!! We should disable Unicode translation if
                        // the console is not active.
                        sx3_game_key_hit(event.key.keysym.sym,
                            event.key.keysym.mod, event.key.state);
                    }
                    break;
                case SDL_KEYUP:
                    sx3_game_key_hit(event.key.keysym.sym,
                        event.key.keysym.mod, event.key.state);
                    break;
                case SDL_MOUSEMOTION:
                    sx3_game_mouse_motion(event.motion.state,
                        event.motion.x, event.motion.y,
                        event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_VIDEORESIZE:
                    resize(event.resize.w, event.resize.h);
                    SDL_SetVideoMode(event.resize.w, event.resize.h, 24,
                        SDL_OPENGL|SDL_RESIZABLE);
                    break;
                case SDL_QUIT:
                    set_game_mode(SX3_GAME_END);
                    break;
            }
        }

        // FIX ME!! dt should be in seconds, not in microseconds, as we
        // have here.
        time = SDL_GetTicks();
        dt = (float)(time - old_time) / 1000.0;
        old_time = time;
        
        sx3_game_animate(dt);
        sx3_game_update(dt);

        sx3_update_screen(e.p, vp.p, vd.p, up.p, dt);
        SDL_GL_SwapBuffers();
    }

    close_game();
    return;
}


// Called by the console when it wants to signal to us to quit
// TODO: We should give this to the console via a function pointer
void console_quit(const char * p1, const char * p2)
{
    set_game_mode(SX3_GAME_END);
}
