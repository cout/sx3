// File: sx3_title.c
// Author: Paul Brannan
//
// Title screen

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <textures.h>
#include "sx3_state.h"
#include "sx3_game.h"
#include "sx3_files.h"
#include "sx3_misc.h"

// The amount of time to fade in/out, in milliseconds
#define FADE_TIME 1000.0

static int title_texture = 0;
static float fade = 0.0;
static Uint32 old_time = 0;

void sx3_title_display(void)
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    if(title_texture != 0)
    {
        glColor3f(fade, fade, fade);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 1, 0, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBindTexture(GL_TEXTURE_2D, title_texture);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 1); glVertex2i(0, 0);
            glTexCoord2i(1, 1); glVertex2i(1, 0);
            glTexCoord2i(1, 0); glVertex2i(1, 1);
            glTexCoord2i(0, 0); glVertex2i(0, 1);
        glEnd();
    }

    SDL_GL_SwapBuffers();

    switch(get_game_mode())
    {
        case SX3_TITLE_SCREEN_IN:
            if(fade < 1.0)
            {
                Uint32 time = SDL_GetTicks();
                float dt = time - old_time;
                fade = fade + dt/FADE_TIME;
                old_time = time;
            }
            else
            {
                fade = 1.0;
                set_game_mode(SX3_TITLE_SCREEN);
            }
            break;
        case SX3_TITLE_SCREEN_OUT:
            if(fade > 0.0)
            {
                Uint32 time = SDL_GetTicks();
                float dt = time - old_time;
                fade = fade - dt/FADE_TIME;
                old_time = time;
            }
            else
            {
                set_game_mode(SX3_TITLE_SCREEN_DONE);
            }
            break;
    }
}

void sx3_title()
{
    SDL_Event event;

    set_game_mode(SX3_TITLE_SCREEN_IN);
    title_texture = bind_tex(SX3_TITLE_SCREEN_BITMAP, 0);

    // Fade the title screen in 
    old_time = SDL_GetTicks();
    while(get_game_mode() == SX3_TITLE_SCREEN_IN)
    {
        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP)
                break;
            if(event.type == SDL_QUIT) exit(0);
        }
        sx3_title_display();
    }

    // Let the title screen sit there without eating CPU
    while(get_game_mode() != SX3_TITLE_SCREEN_OUT)
    {
        if(SDL_WaitEvent(&event))
        {
            if(event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP)
            {
                set_game_mode(SX3_TITLE_SCREEN_OUT);
                break; // we don't want to call display yet
            }
            if(event.type == SDL_QUIT) exit(0);
            if(event.type == SDL_ACTIVEEVENT ||
               event.type == SDL_SYSWMEVENT  ||
               event.type == SDL_VIDEORESIZE)
            {
                sx3_title_display();
            }
        }
    }

    // Fade the title screen out
    old_time = SDL_GetTicks();
    while(get_game_mode() == SX3_TITLE_SCREEN_OUT)
    {
        sx3_title_display();
        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) exit(0);
        }
    }

    // Cleaup up
    free_tex(title_texture);
}

