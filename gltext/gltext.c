/* gltext.c -- a text drawing module
 * Paul Brannan 7/17/2000
 */

#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <textures.h>
#include "gltext.h"

// ---------------------------------------------------------------------------
// Global data and macros
// ---------------------------------------------------------------------------

#define GLT_END        -1
#define GLT_DOT        -2
#define GLT_CR         -3
#define GLT_LF         -4
#define GLT_BACK       -5
#define GLT_UP         -6

#define BITMAP_CHAR_HEIGHT 14
#define BITMAP_CELL_HEIGHT 14

#define MKSTR(x) #x
#define MKFONT(name, size) "*" ## name ## "--" ## MKSTR(size) ## "*"

static const int alphabet[][20] = {
    { GLT_END },                                                        // 0
    { GLT_END },
    { GLT_END },
    { GLT_END },
    { GLT_END },
    { GLT_END },                                                        // 5
    { GLT_END },
    { GLT_END },                                                        // <BEL>
    { GLT_BACK },                                                       // <BS>
    { GLT_END },                                                        // <HT>
    { GLT_LF },                                                         // <LF>
    { GLT_END },                                                        // <VT>
    { GLT_END },                                                        // <FF>
    { GLT_CR },                                                         // <CR>
    { GLT_END },
    { GLT_END },                                                        // 15
    { GLT_END },
    { GLT_END },
    { GLT_END },
    { GLT_END },
    { GLT_END },                                                        // 20
    { GLT_END },
    { GLT_END },
    { GLT_END },
    { GLT_END },
    { GLT_END },                                                        // 25
    { GLT_END },
    { GLT_END },                                                        // <ESC> 
    { GLT_END },
    { GLT_END },
    { GLT_END },                                                        // 30
    { GLT_END },
    { GLT_END },                                                        // space
    { 6, 9, GLT_DOT, 10, GLT_END},                                      // !
    { 6, 7, 2, GLT_UP, 12, 13, 8, GLT_END},                             // "
    { 7, 4, GLT_UP, 13, 10, GLT_UP, 2, 14, GLT_UP, 3, 15, GLT_END},     // #
    {13, 1, 2, 14, 15, 3, GLT_UP, 6, 10, GLT_END},                      // $
    {13, 3, GLT_UP, 0, 6, 7, 1, 0, GLT_UP, 9, 15, 16, 10, 9, GLT_END},  // %
    {16, 1, 6, 13, 3, 10, 15, GLT_END},                                 // &
    {12, 13, 8, GLT_END},                                               // '
    {6, 1, 3, 10, GLT_END},                                             // (
    {6, 13, 15, 10, GLT_END},                                           // )
    {1, 15, GLT_UP, 3, 13, GLT_UP, 7, 9, GLT_END},                      // *
    {7, 9, GLT_UP, 2, 14, GLT_END},                                     // +
    {15, 16, 11, GLT_END},                                              // ,
    {2, 14, GLT_END},                                                   // -
    {GLT_DOT, 4, GLT_END},                                              // .
    {12, 4, GLT_END},                                                   // /
    { 1,  6, 13, 15, 10,  3,  1, GLT_END},                              // 0
    { 1,  6, 10,  4, 16, GLT_END},                                      // 1
    { 0, 12, 14,  2,  4, 16, GLT_END},                                  // 2
    { 0, 12, 14,  2, 14, 16,  4, GLT_END},                              // 3
    { 0,  2, 14, 12, 16, GLT_END},                                      // 4
    {12,  0,  2, 14, 16,  4, GLT_END},                                  // 5
    { 0,  4, 16, 14,  2, GLT_END},                                      // 6
    { 0, 12, 16, GLT_END},                                              // 7
    { 0, 12, 16,  4,  0,  2, 14, GLT_END},                              // 8
    {14,  2,  0, 12, 16, GLT_END},                                      // 9
    {GLT_DOT, 7, GLT_DOT, 9, GLT_END},                                  // :
    {GLT_DOT, 7, 9, 10, 5, GLT_END},                                    // ;
    {12, 2, 16, GLT_END},                                               // <
    {1, 13, GLT_UP, 2, 14, GLT_END},                                    // =
    {0, 14, 4, GLT_END},                                                // >
    {1, 6, 13, 14, 8, 9, GLT_DOT, 10, GLT_END},                         // ?
    {15, 9, 8, 14, 16, 10, 3, 2, 7, 13, GLT_END},                       // @
    { 4,  0, 12, 16, 14,  2, GLT_END},                                  // A
    { 4,  0,  6, 13, 14,  2,  8, 15, 16,  4, GLT_END},                  // B
    {12,  0,  4, 16, GLT_END},                                          // C
    { 0,  6, 13, 15, 10,  4,  0, GLT_END},                              // D
    {12,  0,  2,  8,  2,  4, 16, GLT_END},                              // E
    {12,  0,  4,  2,  8, GLT_END},                                      // F
    {12,  0,  4, 16, 15,  9, GLT_END},                                  // G
    { 0,  4,  2, 14, 12, 16, GLT_END},                                  // H
    { 0, 12,  6, 10,  4, 16, GLT_END},                                  // I
    { 0, 12, 16,  4,  3, GLT_END},                                      // J
    { 0,  4,  2, 12,  2, 16, GLT_END},                                  // K
    { 0,  4, 16, GLT_END},                                              // L
    { 4,  0,  8, 12, 16, GLT_END},                                      // M
    { 4,  0,  16, 12, GLT_END},                                         // N
    { 0, 12, 16,  4,  0, GLT_END},                                      // O
    { 4,  0, 12, 14,  2, GLT_END},                                      // P
    { 0, 12, 15, 10, 4, 0, GLT_UP, 9, 16, GLT_END},                     // Q
    { 4,  0, 12, 14,  2,  8, 16, GLT_END},                              // R
    {12,  6,  1, 2, 14, 15, 10,  4, GLT_END},                           // S
    { 0, 12,  6, 10, GLT_END},                                          // T
    { 0,  4, 16, 12, GLT_END},                                          // U
    { 0, 10, 12, GLT_END},                                              // V
    { 0,  4, 10,  8, 10, 16, 12, GLT_END},                              // W
    { 0, 16, GLT_UP, 12,  4, GLT_END},                                  // X
    { 0,  8, 12,  8, 10, GLT_END},                                      // Y
    { 0, 12,  4, 16, GLT_END},                                          // Z
    {6, 0, 4, 10, GLT_END},                                             // [
    {0, 16, GLT_END},                                                   // '\'
    {6, 12, 16, 10, GLT_END},                                           // ] 
    {1, 6, 13, GLT_END},                                                // ^
    {4, 16, GLT_END},                                                   // _
    {6, 7, 14, GLT_END},                                                // `
    {15,  8,  3, 10, 15, 14, 16, GLT_END},                              // a
    { 0,  4, 10, 15,  8,  2, GLT_END},                                  // b
    {14,  2,  4, 16, GLT_END},                                          // c
    {12, 16, 10,  3,  8, 14, GLT_END},                                  // d
    { 3, 15, 14,  2,  4, 16, GLT_END},                                  // e
    {13,  6,  1,  4,  2,  8, GLT_END},                                  // f
    { 5, 17, 14,  3, 10, 16, GLT_END},                                  // g
    { 0,  4,  2, 14, 16, GLT_END},                                      // h
    { 8, 10, GLT_END, GLT_DOT, 7},                                      // i
    {14, 16, 11,  5,  4, GLT_END, GLT_DOT, 13},                         // j
    { 0,  4,  3, 14,  3, 16, GLT_END},                                  // k
    { 6, 10, GLT_END},                                                  // l
    { 4,  2,  8,  9,  8, 14, 16, GLT_END},                              // m
    { 4,  2,  3,  8, 14, 16, GLT_END},                                  // n
    { 2,  4, 16, 14,  2, GLT_END},                                      // o
    { 5,  2, 14, 15, 10,  4, GLT_END},                                  // p
    {17, 14,  2,  3, 10, 16, GLT_END},                                  // q
    { 2,  4,  3,  8, 14, GLT_END},                                      // r
    {14,  2,  3, 15, 16,  4, GLT_END},                                  // s
    { 6, 10, GLT_UP,  2, 14, GLT_END},                                  // t
    { 2,  4, 10, 15, 14, 16, GLT_END},                                  // u
    { 2, 10, 14, GLT_END},                                              // v
    { 2,  4, 10,  9, 10, 16, 14, GLT_END},                              // w
    { 2, 16,  9,  4, 14, GLT_END},                                      // x
    { 2,  3, 10, 15, 14, 17,  5, GLT_END},                              // y
    { 2, 14,  4, 16, GLT_END},                                          // z
    { 6, 1, 8, 2, 8, 3, 10, GLT_END},                                   // {
    { 6, 11, GLT_END},                                                  // |
    {6, 13, 8, 14, 8, 15, 10, GLT_END},                                 // }
    {1, 6, 7, 12, GLT_END},                                             // ~
    {GLT_END},                                                          // DEL
    {GLT_END},
};

static const float verts[][2] = {
    {0.0, 1.5},        // 0
    {0.0, 1.125},
    {0.0, 0.75},
    {0.0, 0.375},
    {0.0, 0.0},
    {0.0, -0.375},     // 5
    {0.375, 1.5},
    {0.375, 1.125},
    {0.375, 0.75},
    {0.375, 0.375},
    {0.375, 0.0},      // 10
    {0.375, -0.375},
    {0.75, 1.5},
    {0.75, 1.125},
    {0.75, 0.75},
    {0.75, 0.375},     // 15
    {0.75, 0.0},
    {0.75, -0.375}
};

#ifdef WIN32
    static  PIXELFORMATDESCRIPTOR pfd =    
        sizeof(PIXELFORMATDESCRIPTOR),      // Size of this structure
        1,                                  // Version Number
        PFD_DRAW_TO_WINDOW |                // Format Must Support Window
        PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
        PFD_TYPE_RGBA,                      // Request An RGBA Format
        bits,                               // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
        0,                                  // No Alpha Buffer
        0,                                  // Shift Bit Ignored
        0,                                  // No Accumulation Buffer
        0, 0, 0, 0,                         // Accumulation Bits Ignored
        16,                                 // 16Bit Z-Buffer (Depth Buffer)
        0,                                  // No Stencil Buffer
        0,                                  // No Auxiliary Buffer
        PFD_MAIN_PLANE,                     // Main Drawing Layer
        0,                                  // Reserved
        0, 0, 0                             // Layer Masks Ignored
    };
#endif

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

static void init_bitmap_font(gltContext *g, int font) {
    char * fontname;

    g->font_lists[font] = glGenLists(128);
#ifdef WIN32
    switch(font) {
        case GLT_HELVETICA: fontname = "Arial";    break;
        case GLT_TIMES:     fontname = "Times";    break;
        case GLT_COURIER:   fontname = "Courier";  break;
        case GLT_FIXED:     fontname = "Terminal"; break;
    }
    winfont = CreateFont(
        -CHAR_HEIGHT,               // 14-point character height
        0,                          // default width
        0,                          // 0 angle of escapement
        0,                          // 0 orientation angle
        FW_NORMAL,                  // normal font weight
        FALSE,                      // not italic
        FALSE,                      // not underline
        FALSE,                      // not strikeout
        ANSI_CHARSET,               // character set identifier
        OUT_TT_PRECIS,              // prefer truetype fonts
        CLIP_DEFAULT_PRECIS,        // default clipping
        ANTIALIASED_QUALITY,        // antialiased if we can get it
        FW_DONTCARE|DEFAULT_PITCH,  // default family and pitch
        fontname);
    selectObject(g->hDC, font);
    wglUseFontBitmaps(g->hDC, 0, 127, g->font_lists[font]);
#else
    switch(font) {
        case GLT_HELVETICA:
            fontname = MKFONT("helvetica-medium-r-normal", BITMAP_CHAR_HEIGHT);
            break;
        case GLT_TIMES:
            fontname = MKFONT("times-medium-r-normal", BITMAP_CHAR_HEIGHT);
            break;
        case GLT_COURIER:
            fontname = MKFONT("courier-medium-r-normal", BITMAP_CHAR_HEIGHT);
            break;
        case GLT_FIXED:
            fontname = "fixed";
            break;
    }
    glXUseXFont(
        XLoadFont(g->display, fontname),
        0, 127,
        g->font_lists[font]);
#endif
}

static void close_glt() {
    // TODO
}

#define INIT_ERROR_CHECK(x) \
    do { \
        if(!(x)) { \
            close_glt(); \
            return 0; \
        } \
    } while(0)

static int init_glt(gltContext *g) {
#ifdef WIN32
    /*
    g->hInstance = GetModuleHandle(NULL);
    // TODO: We must register a wc before we can create a window.  This
    // *should* already be done.  In fact, we might be able to get our hWnd
    // from wgl or sdl.
    g->hWnd = CreateWindowEx(
        WS_EX_APPWINDOW,            // Extended Style For The Window
        "OpenGL",                   // Class Name
        "",                         // Window Title
        WS_CLIPSIBLINGS |           // Required Window Style
        WS_CLIPCHILDREN |           // Required Window Style
        WS_POPUP,                   // Selected Window Style
        0, 0,                       // Window Position
        16, 16,
        NULL,                       // No Parent Window
        NULL,                       // No Menu
        g->hInstance,               // Instance
        NULL));                     // Don't Pass Anything To WM_CREATE
    INIT_ERROR_CHECK(g->hWnd);
    INIT_ERROR_CHECK(g->hDC = GetDC(g->hWnd));
    INIT_ERROR_CHECK(PixelFormat = ChoosePixelFormat(g->hDC, &pfd));
    INIT_ERROR_CHECK(g->PixelFormat);
    INIT_ERROR_CHECK(SetPixelFormat(g->hDC, g->PixelFormat, &pfd));
    INIT_ERROR_CHECK(g->hRC = wglCreateContext(g->hDC));
    INIT_ERROR_CHECK(wglMakeCurrent(g->hDC, g->hRC));
    */
#else
    g->display = (Display*)glXGetCurrentDisplay();
    INIT_ERROR_CHECK(g->display);
#endif
    return 1;
}

gltContext* gltNewContext() {
    gltContext *g;
    int j;

    g = malloc(sizeof(gltContext));
    if(!g) return 0;

    if(init_glt(g)) {
        for(j = GLT_NONE + 1; j < GLT_MAX_FONTS; ++j) {
            init_bitmap_font(g, j);
        }
    } else {
        free(g);
        return 0;
    }

    g->x = 0;
    g->y = 0;
    g->in_draw = 0;
    g->current_bitmap_font = GLT_NONE;
    g->texture_font = 0;

    return g;
}

void gltFreeContext(gltContext *g) {
    close_glt();

    glDeleteLists(g->font_lists[GLT_HELVETICA], 126);
    glDeleteLists(g->font_lists[GLT_TIMES], 126);
    glDeleteLists(g->font_lists[GLT_COURIER], 126);

    if(g) free(g);
}

// ---------------------------------------------------------------------------
// Wireframe fonts
// ---------------------------------------------------------------------------

void gltWireChar(gltContext *g, unsigned char c) {
    const int *i;

    if(c > 128) return;

    glBegin(GL_LINE_STRIP);
    for(i = alphabet[c]; *i != GLT_END; i++) {
        switch(*i) {
            case GLT_DOT:
                glEnd();
                glBegin(GL_POINTS);
                i++;
                glVertex2f(
                    g->x + g->font_x * (0.1 + 0.9*verts[*i][0]),
                    g->y + g->font_y * (1.5 - 0.9*verts[*i][1]));
                glEnd();
                glBegin(GL_LINE_STRIP);
                break;
            case GLT_LF:
                glEnd();
                glBegin(GL_LINE_STRIP);
                g->x -= g->font_x * 1.0;
                g->y += g->font_y * 2.0;
                break;
            case GLT_CR:
                glEnd();
                glBegin(GL_LINE_STRIP);
                g->x = -g->font_x * 1.0;
                break;
            case GLT_BACK:
                glEnd();
                glBegin(GL_LINE_STRIP);
                g->x -= g->font_x * 2.0;
                break;
            case GLT_UP:
                glEnd();
                glBegin(GL_LINE_STRIP);
                break;
            default:
                glVertex2f(
                    g->x + g->font_x * (0.1 + 0.9*verts[*i][0]),
                    g->y + g->font_y * (1.5 - 0.9*verts[*i][1]));
        }
    }
    glEnd();

    g->x += g->font_x * 1.0;
}

void gltWireString(gltContext *g, const unsigned char *s) {
    g->in_draw = 1;
    for(; *s != 0; s++) {
        gltWireChar(g, *s);
    }
    g->in_draw = 0;
}

// ---------------------------------------------------------------------------
// Bitmap fonts
// ---------------------------------------------------------------------------

void gltSelectBitmapFont(gltContext* g, int font) {
    g->current_bitmap_font = font;
}

static void update_coords(gltContext *g) {
    GLdouble d[4];
    GLdouble model_matrix[16];
    GLdouble proj_matrix[16];
    GLint viewport[4];
    GLdouble x, y, z;

    glGetDoublev(GL_CURRENT_RASTER_POSITION, d);
    glGetDoublev(GL_MODELVIEW_MATRIX, model_matrix);
    glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluUnProject(
        d[0], d[1], d[2],
        model_matrix, proj_matrix, viewport,
        &x, &y, &z);
    g->x = x;
    g->y = y - BITMAP_CELL_HEIGHT;
}

void gltBitmapChar(gltContext *g, unsigned char c) {
    switch(c) {
        case '\0':
            break;
        case '\r':
            g->x = 0.0;
            break;
        case '\n':
            g->x = 0.0;
            g->y += BITMAP_CELL_HEIGHT;
            break;
        default:
            glRasterPos2f(g->x, g->y + BITMAP_CELL_HEIGHT);
            glCallList(g->font_lists[g->current_bitmap_font]+c);
            update_coords(g);
    }
}

static const unsigned char* strfind(const unsigned char *s,
        const unsigned char *c) {
    const unsigned char *t;
    for(; *s != 0; s++) {    
        for(t = c; *t != 0; t++) if(*s == *t) return s;
    }
    return NULL;
}

void gltBitmapList(gltContext* g, const unsigned char* s, int n) {
    if (n > 0) {
        glRasterPos2f(g->x, g->y + BITMAP_CELL_HEIGHT);
        glListBase(g->font_lists[g->current_bitmap_font]);
        glCallLists(n, GL_UNSIGNED_BYTE, s);
        update_coords(g);
    }
}
    
void gltBitmapString(gltContext* g, const unsigned char* s) {
    const unsigned char *substr;
    if((substr = strfind(s, "\r\n"))) {
        unsigned char c = *substr;
        gltBitmapList(g, s, substr - s);
        gltBitmapChar(g, c);
        gltBitmapString(g, substr + 1);
        return;
    }

    gltBitmapList(g, s, strlen(s));
}

// ---------------------------------------------------------------------------
// Textured fonts
// ---------------------------------------------------------------------------

void gltLoadTextureFont(gltContext *g, const char *filename) {
    g->texture_font = bind_tex(filename, 0);
}

void gltTextureChar(gltContext *g, unsigned char c) {
    if(!g->texture_font);

    switch(c) {
        case '\0':
            break;
        case '\r':
            g->x = 0.0;
            break;
        case '\n':
            g->x = 0.0;
            g->y += g->font_y;
            break;
        default:
            glBindTexture(GL_TEXTURE_2D, g->texture_font);
            glEnable(GL_TEXTURE_2D);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            glBegin(GL_QUADS);
            glTexCoord2f(1.0/256.0 * c, 0.0);
            glVertex2f(g->x, g->y);                         // upper left
            glTexCoord2f(1.0/256.0 * c, 1.0);
            glVertex2f(g->x, g->y + g->font_y);             // lower left
            glTexCoord2f(1.0/256.0 * (c + 1), 1.0);
            glVertex2f(g->x + g->font_x, g->y + g->font_y); // lower right
            glTexCoord2f(1.0/256.0 * (c + 1), 0.0);
            glVertex2f(g->x + g->font_x, g->y);             // upper right
            glEnd();

            g->x += g->font_x;
    }
}

void gltTextureString(gltContext *g, const unsigned char *s) {
    for(; *s != 0; ++s) gltTextureChar(g, *s);
}

// ---------------------------------------------------------------------------
// Miscellaneous functions
// ---------------------------------------------------------------------------

void gltFontSize(gltContext* g, float x, float y) {
    g->font_x = x;
    g->font_y = y;
}

