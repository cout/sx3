// File: sx3_gui.c
// Author: Paul Brannan
//
// Graphical user interface module.

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "sx3.h"
#include "sx3_gui.h"
#include "sx3_tanks.h"
#include "sx3_weapons.h"
#include "sx3_global.h"
#include "sx3_files.h"

// FIX ME!! For some reason, sx3_tanks.h must be included before gltext.h.
// I would imagine that this is due to some odd macro in gltext.h.
#include <gltext.h>

// ===========================================================================
// Global variables
// ===========================================================================

static gltContext *glt_context;


// ===========================================================================
// Function definitions
// ===========================================================================

void sx3_init_gui()
{
    glt_context = gltNewContext();
    gltSelectFont(glt_context, GLT_TIMES);
    gltFontSize(glt_context, 10, 10);
}

void sx3_close_gui()
{
    gltFreeContext(glt_context);
}

// sx3_draw_text draws text without first setting the projection matrices
// FIX ME!! For the moment we are ignoring 'font'
SX3_ERROR_CODE sx3_draw_text(int x, int y, char* s, enum sx3_font_types font)
{
    glPushMatrix();
    glLoadIdentity();
    glt_context->x = x;
    glt_context->y = y;
    gltWireString(glt_context, s);
    glPopMatrix();
    /*
    glRasterPos2d(x, y);
    gltBitmapString(glt_context, s);
    */

    return SX3_ERROR_SUCCESS;    
}

SX3_ERROR_CODE sx3_display_text(int x, int y, char* s, enum sx3_font_types font)
{
    SX3_ERROR_CODE retcode;

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_window_size.x, g_window_size.y, 0);
    glMatrixMode(GL_MODELVIEW);

    retcode = sx3_draw_text(x, y, s, font);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    return retcode;
}

// Warning!  This function not thread-safe!
void sx3_draw_hud(float dt)
{
    static char frame_text[32];
    static char s[32];
    static int frames = 0;
    struct Tank *t = &g_tanks[g_current_tank];

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_window_size.x, g_window_size.y, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // spit out frame rate.
    if(g_display_frame_rate)
    {
        g_frame_time += dt;

        frames++;
        if(frames > 17)
        {
            sprintf(frame_text, "%.2f fps", (float)frames/g_frame_time);
            g_frame_time = 0;
            frames = 0;
        }
        sx3_draw_text(5, g_window_size.y - 20, frame_text, SX3_DEFAULT_FONT);
    }

    // Display the current tank numer
    // FIX ME!! We aren't writing text in the correct y-position
    sprintf(s, "Tank %d", g_current_tank+1);
    sx3_draw_text(5, 5, s, SX3_DEFAULT_FONT);

    // Draw the tank's power
    glColor3f(0.7, 0.0, 0.0);
    glRectf(
        g_window_size.x-25.0,
        g_window_size.y - 30.0,
        g_window_size.x- 5.0,
        g_window_size.y - 30.0-t->s.power/t->s.max_power*100.0
    );
    sprintf(s, "%5.0f", t->s.power);
    sx3_draw_text(g_window_size.x-55, g_window_size.y - 25, s, SX3_DEFAULT_FONT);

    // And the tank's energy
    glColor3f(0.0, 0.8, 0.0);
    glRectf(
        g_window_size.x-25.0,
        g_window_size.y - 140.0,
        g_window_size.x- 5.0,
        g_window_size.y - 140.0-t->s.energy/t->s.max_energy*100.0
    );
    sprintf(s, "%5.0f", t->s.energy);
    sx3_draw_text(g_window_size.x-55, g_window_size.y - 135, s, SX3_DEFAULT_FONT);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

