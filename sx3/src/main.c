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

int main(int argc, char *argv[])
{
    int i;
    int fullScreen = 0;

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
            fullScreen = 1;
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
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if(SDL_SetVideoMode(640, 480, 24, SDL_OPENGL|SDL_RESIZABLE|
            (fullScreen?  SDL_FULLSCREEN:0)) == NULL)
    {
        fprintf(stderr, "Unable set set GL mode\n");
        exit(1);
    }

    // Set some other misc. options
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    sx3_title();
    sx3_game();

    return 0;
}

