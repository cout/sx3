// Textures module; uses SDL_image
// Author: Paul Brannan
// FIX ME!! We should use a hash table to determine if a texture has already
// been loeded (so as not to load a texture twice)
// FIX ME!! We no longer support loading of images with palettes (the imlib
// version of this module did)

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "textures.h"

#define USE_MIPMAPS

// b2scale converts a number to the next largest power of two (or leaves
// it alone if it is already a power of 2)
int b2scale(int x) {
    int j, o = x;
    if(x == 0) return 0;
    for(j = 0; x != 1; x >>= 1) j++;
    j = x << (j+1);
    if(j == 2*o) return o; else return j;
}

// bind_tex_private is our sorry attempt at C exception handling.  The
// SDL_Surface must be freed before we return to the user, and there are
// a number of exceptions that can occur along the way here.
static int bind_tex_private(SDL_Surface *image, int texnum) {
    SDL_PixelFormat *fmt;
    unsigned char *data, *scaled_data;
    int w, h, x, y, bpp, format;

    // Find the dimensions of the image to the nearest power of 2
    w = b2scale(image->w);
    h = b2scale(image->h);
    if(w == 0 || h == 0) return 0;

    // support 24 & 32 bit images
    bpp = image->format->BitsPerPixel;
    if(bpp != 24 && bpp != 32) return 0;
    format = (bpp == 24) ? GL_RGB : GL_RGBA;
  
    data = malloc(image->w * image->h * bpp / 8);
    fmt = image->format;
    if(!data) return 0;
    for(y = 0; y < image->h; y++) {
        for(x = 0; x < image->w; x++) {
            Uint8 *bufp;

            bufp = (Uint8 *)image->pixels + y*image->pitch + (bpp / 8) * x;
            // For some odd reason, Mingw32 appears to (incorrectly)
            // swap the red and blue pixel values.
#ifdef __MINGW32__
            data[(bpp/8) * (y*image->w+x) + 0] = bufp[fmt->Bshift/8];
            data[(bpp/8) * (y*image->w+x) + 1] = bufp[fmt->Gshift/8];
            data[(bpp/8) * (y*image->w+x) + 2] = bufp[fmt->Rshift/8];
#else
            data[(bpp/8) * (y*image->w+x) + 0] = bufp[fmt->Rshift/8];
            data[(bpp/8) * (y*image->w+x) + 1] = bufp[fmt->Gshift/8];
            data[(bpp/8) * (y*image->w+x) + 2] = bufp[fmt->Bshift/8];
#endif
            // why on earth are the alpha values reversed?
            if(bpp == 32) {
                data[(bpp/8) * (y*image->w+x) + 3] = 255 - bufp[fmt->Ashift/8];
            }
        }
    }

    // Scale the image appropriately
    if(w != image->w || h != image->h) {
        scaled_data = malloc(w*h*bpp/8);
        if(!scaled_data) return 0;
        gluScaleImage(format,
            image->w, image->h, GL_UNSIGNED_BYTE, data,
            w, h, GL_UNSIGNED_BYTE, scaled_data);
        free(data);
    } else {
        scaled_data = data;
    }

    // Get an unused texture ID
    while(texnum == 0) glGenTextures(1, (GLuint*)&texnum);

    // Bind the texture and store it in memory
    glBindTexture(GL_TEXTURE_2D, texnum);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

// FIX ME!! This should be a run-time option.
#ifdef USE_MIPMAPS
    gluBuild2DMipmaps(GL_TEXTURE_2D, format, w, h, format, GL_UNSIGNED_BYTE,
        scaled_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE,
        scaled_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif

    // Note that if GL_REPEAT is not the desired mode, then the texture
    // will look funny, and will not seam properly.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    free(scaled_data);
    return texnum;
}

// bind_tex loads an image from a file and binds the image to a texture number.
int bind_tex(const char* filename, int texnum) {
    SDL_Surface *image;
    int retval;

    image = IMG_Load(filename);
    if(!image) {
        fprintf(stderr, "Error loading image %s\n", filename);
        return 0;
    }

    retval = bind_tex_private(image, texnum);
    SDL_FreeSurface(image);
    return retval;
}

// For now, this is a wrapper into the corresponding GL function.  This could
// change if we add a hash table.
void free_tex(int texnum) {
    glDeleteTextures(1, (GLuint*)&texnum);
}
