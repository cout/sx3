// File: main.c
// Author: Paul Brannan
//
// Here we initialize SDL, the Sx3 engine, display a title screen, process
// the menu, and enter the main game loop.  Lastly, we do some cleanup
// before the program exits.

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sx3_misc.h"
#include "sx3_global.h"
#include "sx3_tanks.h"
#include "sx3_title.h"
#include "sx3_math.h"
#include "sx3_game.h"

CLEANUP_TYPE CleanUp(void)
{
    SDL_Quit();
    CLEANUP_RET(0);
}

struct Video_Mode {
    int red_size;
    int green_size;
    int blue_size;
    int depth_size;
    int double_buffer;
    int width;
    int height;
    int bpp;
};

const struct Video_Mode video_modes[] = {
/*
    { 8, 8, 8, 32, 1, 1024, 768, 24 },
    { 8, 8, 8, 16, 1, 1024, 768, 24 },
    { 8, 8, 8, 32, 1,  800, 600, 24 },
    { 8, 8, 8, 16, 1,  800, 600, 24 },
    { 8, 8, 8, 32, 1,  640, 480, 24 },
    { 8, 8, 8, 16, 1,  640, 480, 24 },
*/

/*
    { 5, 6, 5, 32, 1, 1024, 768, 16 },
    { 5, 6, 5, 16, 1, 1024, 768, 16 },
    { 5, 6, 5, 32, 1,  800, 600, 16 },
    { 5, 6, 5, 16, 1,  800, 600, 16 },
*/
    { 5, 6, 5, 32, 1,  640, 480, 16 },
    { 5, 6, 5, 16, 1,  640, 480, 16 },

    { 0, 0, 0,  0, 0,    0,   0,  0 }
};

int try_gl_mode(int fullscreen, const struct Video_Mode *vid_mode)
{
    int sdl_fullscreen = fullscreen?SDL_FULLSCREEN:0;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, vid_mode->red_size);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, vid_mode->green_size);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, vid_mode->blue_size);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, vid_mode->depth_size);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, vid_mode->double_buffer);

    printf("Trying video mode %dx%dx%d, RGB bits=[%d %d %d], "
           "%d-bit depth buffer\n",
            vid_mode->width, vid_mode->height, vid_mode->bpp,
            vid_mode->red_size, vid_mode->green_size, vid_mode->blue_size,
            vid_mode->depth_size);

    if(SDL_SetVideoMode(vid_mode->width, vid_mode->height, vid_mode->bpp,
            SDL_OPENGL | SDL_RESIZABLE | sdl_fullscreen) != NULL)
        return 1;
    else
        return 0;
}

void set_gl_mode(int fullscreen)
{
    const struct Video_Mode *vid_mode = video_modes;

    for(; vid_mode->bpp != 0; vid_mode++)
    {
        if(try_gl_mode(fullscreen, vid_mode))
        {
            printf("Video mode OK\n");
            return;
        }
    }
    fprintf(stderr, "Unable set set GL mode\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    int i;
    int fullscreen = 0;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to initialize SDL\n");
        exit(1);
    }

    // Set our quit function so we can clean up properly
    if(!ONEXIT (CleanUp))
    {
        fprintf(stderr, "Unable to allocate CleanUp() function to exit list!\n"
            "EXITING...\n\n");
        return 1;
    }

    // Parse the command line
    for(i = 1; --argc; i++)
    {
        if(!strcmp("-huge",argv[i]))
        {
            fullscreen = 1;
        }
        else if(!strcmp("-g_view_radius",argv[i]))
        {
            g_view_radius = atoi(argv[--argc,++i]);
            if(g_view_radius<1)
            {
                fprintf(stderr,"g_view_radius must be greater than 0.\n");
                exit(1);
            }
        }
        else
        {
            fprintf(stderr,"Unrecognized argument: %s\n",argv[i]);
            fprintf(stderr,"SYNTAX:  %s [-huge] [-g_view_radius <integer>]\n", argv[0]);
            exit(1);
        }
    }

    // Set our desired GL attributes.  I'm not sure if this is necessary,
    // and I'm not sure what happens if the desired attributes cannot be
    // negotiated.
    set_gl_mode(fullscreen);

    // Set some other misc. options
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    sx3_title();
    sx3_game();

    return 0;
}

