#ifndef GLTEXT_H
#define GLTEXT_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef WIN32
#include <GL/glx.h>
#endif

enum GLT_FONTS {
    GLT_NONE,

    GLT_HELVETICA,
    GLT_TIMES,
    GLT_COURIER,
    GLT_FIXED,

    GLT_MAX_FONTS
};

typedef struct {
    float x, y;
    float font_x, font_y;
    int in_draw;
    int font_lists[GLT_MAX_FONTS];
    int current_bitmap_font;
    int texture_font;

#ifdef WIN32
#if 0
    HGLRC hRC = NULL;
    HDC hDC = NULL;
    HWND hWnd = NULL;
    HINSTANCE hInstance;
    GLuint PixelFormat;
    WNDCLASS wc;
#endif
#else
    Display* display;
#endif
} gltContext;

#ifdef __cplusplus
extern "C" {
#endif

gltContext* gltNewContext();
void gltFreeContext(gltContext* g);

void gltFontSize(gltContext *g, float x, float y);

void gltWireChar(gltContext* g, unsigned char c);
void gltWireString(gltContext* g, const unsigned char* s);

void gltBitmapChar(gltContext* g, unsigned char c);
void gltSelectBitmapFont(gltContext* g, int font);
void gltBitmapString(gltContext* g, const unsigned char* s);

void gltLoadTextureFont(gltContext* g, const char *filename);
void gltTextureChar(gltContext* g, unsigned char c);
void gltTextureString(gltContext* g, const unsigned char* s);

#ifdef __cplusplus
}
#endif

#endif
