#ifndef GLTEXT_H
#define GLTEXT_H

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
	int current_font;
#ifndef WIN32
	Display* display;
#endif
} gltContext;

#ifdef __cplusplus
extern "C" {
#endif

gltContext* gltNewContext();
void gltFreeContext(gltContext* g);
void gltWireChar(gltContext* g, unsigned char c);
void gltWireString(gltContext* g, const unsigned char* s);
void gltSelectFont(gltContext* g, int font);
void gltBitmapChar(gltContext* g, unsigned char c);
void gltBitmapString(gltContext* g, const unsigned char* s);
void gltFontSize(gltContext *g, float x, float y);

#ifdef __cplusplus
}
#endif

#endif
