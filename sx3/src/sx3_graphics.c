// File: sx3_graphics.c
// Author: Marc Bryant
//
// This module contains the main graphics logic for the game

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "sx3_global.h"
#include "sx3_terrain.h"
#include "sx3_misc.h"
#include "sx3_gui.h"
#include "sx3_console.h"
#include "sx3_tanks.h"
#include "sx3_weapons.h"

// FIX ME!! Is this the proper place for this?
static int shield_list;

// ===========================================================================
// FUnction definitions
// ===========================================================================

void sx3_draw_projectiles()
{
    int j;
    struct Projectile *p;

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(5.0);

    glBegin(GL_POINTS);
    for(j = 0; j < g_num_projectiles; j++)
    {
        p = &g_projectiles[j];
        glVertex3fv(p->o.props.position);
    }
    glEnd();
        
}

void sx3_draw_explosions()
{
    int j, i;
    struct Explosion *e;
    Vector v;

    glPointSize(2.0);

    glBegin(GL_POINTS);
    for(j = 0; j < g_num_explosions; j++)
    {
        e = &g_explosions[j];
        glColor3f((float)rand()/RAND_MAX,
                  (float)rand()/RAND_MAX,
                  (float)rand()/RAND_MAX);
        for(i = 0; i < 1000; i++)
        {
            v[0] = 2.0 * rand() / RAND_MAX - 1.0;
            v[1] = 2.0 * rand() / RAND_MAX - 1.0;
            v[2] = 2.0 * rand() / RAND_MAX - 1.0;
            v[3] = 0.0;
            v_norm(v);
            vc_mul(v, e->props.radius);
            vv_add(v, e->props.position);
            glVertex3fv(v);
        }
    }
    glEnd();
}

void sx3_draw_tanks ( void)
{
    int count = 0;
    struct Tank *t;

    glEnable(GL_TEXTURE_2D);

    // display each tank
    while (g_tanks [count].id != -1)
    {
        t = &g_tanks[count];

        glPushMatrix ();

        glColor3f (1.0, 1.0, 1.0); 

        // Draw the model
        glTranslatef(t->o.props.position[0],
                     t->o.props.position[1],
                     t->o.props.position[2]);
        glCallList(t->m.model.framestart);

        // Draw the shield
        glPushMatrix();
        glRotatef(180.0, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX,
            (float)rand()/RAND_MAX);
        glScalef(t->o.props.radius, t->o.props.radius, t->o.props.radius);
        glCallList(shield_list);
        glPopMatrix();

        // Draw the turret
        glTranslatef(-t->m.turret_base[0],
                     -t->m.turret_base[1],
                     -t->m.turret_base[2]);
        glRotatef(t->s.turret_angle, 0.0, 1.0, 0.0);
        glTranslatef(t->m.turret_base[0],
                     t->m.turret_base[1],
                     t->m.turret_base[2]);
        glCallList(t->m.turret.framestart);

        // Draw the weapon
        glTranslatef(-t->m.weapon_base[0],
                     -t->m.weapon_base[1],
                     -t->m.weapon_base[2]);
        glRotatef(t->s.weapon_angle, -1.0, 0.0, 0.0);
        glTranslatef(t->m.weapon_base[0],
                     t->m.weapon_base[1],
                     t->m.weapon_base[2]);
        glCallList(t->m.weapon.framestart);

        glPopMatrix ();

        count++;
    }

}

SX3_ERROR_CODE sx3_update_screen(
    struct Point eye_point,
    struct Point view_point,
    struct Point view_dir,
    struct Point up_vector,
    float dt)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up the view
    if (!g_sat_view_on) 
          gluLookAt(eye_point.x,eye_point.y,eye_point.z,
              view_point.x,view_point.y,view_point.z,
              up_vector.x,up_vector.y,up_vector.z);
    else
        gluLookAt(
              eye_point.x,
              sx3_interpolated_terrain_height (eye_point.x, eye_point.z, 1) +
                  g_sat_altitude,
              eye_point.z,
              eye_point.x,
              sx3_interpolated_terrain_height (eye_point.x, eye_point.z, 1),
              eye_point.z,
              0.0, 0.0, -1.0 );
    
    // Draw the scene
    if(g_terrain_lights)
    {
        sx3_draw_terrain_lights();
        glEnable(GL_LIGHTING);
    }
    glColor3f(1.0, 1.0, 1.0);
    sx3_draw_terrain(eye_point, view_dir, up_vector);

    glDisable(GL_LIGHTING);                    // No lighting needed here
    sx3_draw_tanks();                          // Draw the tanks
    sx3_draw_projectiles();                    // Draw the projectiles
    sx3_draw_explosions();                     // Draw the explosions
    sx3_draw_hud(dt);                          // Display the HUD
    sx3_console_refresh_display ();            // Refresh the console

    return SX3_ERROR_SUCCESS;
}

void sx3_gl_settings()
{
    // Fog
    switch (g_fog_type)
    {
        case 0:
            glDisable (GL_FOG);
            break;
        case 1:
            glDisable (GL_FOG);
            break;
        case 2:
            glFogf (GL_FOG_START, (g_view_radius-5) * METERS_PER_MAP_GRID);
            glFogf (GL_FOG_END,   (g_view_radius)   * METERS_PER_MAP_GRID);
            glFogf (GL_FOG_MODE, GL_LINEAR);
            glEnable (GL_FOG);
            break;
        case 3:
            glFogf (GL_FOG_DENSITY, 4.0/(float)(g_view_radius*METERS_PER_MAP_GRID));
            glFogf (GL_FOG_MODE, GL_EXP);
            glEnable (GL_FOG);
            break;
        case 4:
            glFogf (GL_FOG_DENSITY, 2.0/(float)(g_view_radius*METERS_PER_MAP_GRID));
            glFogf (GL_FOG_MODE, GL_EXP2);
            glEnable (GL_FOG);
            break;
    }

    // Wireframe
    if (g_terrain_wire)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

}

void resize(int width, int height)
{
    g_window_size.x = width;
    g_window_size.y = height;

    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(g_fov/M_PI*180.0,(double)width/height,0.1,4096.0);
    glMatrixMode(GL_MODELVIEW);
}

void sx3_init_graphics()
{
    int i;
    Vector v;

    // Create a shield call list
    shield_list = glGenLists(1);
    glNewList(shield_list, GL_COMPILE);
    glPointSize(1.0);
    glBegin(GL_POINTS);
    for(i = 0; i < 1000; i++) {
       v[0] = 2.0 * rand() / RAND_MAX - 1.0;
       v[1] = 2.0 * rand() / RAND_MAX - 1.0;
       v[2] = 2.0 * rand() / RAND_MAX - 1.0;
       v[3] = 0.0;
       v_norm(v);
       glVertex3fv(v);
    }
    glEnd();
    glEndList();
}

void sx3_close_graphics()
{
    glDeleteLists(shield_list, 1);
}
