#ifndef TEXTURES_H
#define TEXTURES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_IMLIB
/* A hack for Mesa, since glXGetCurrentDisplay is not defined */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
Display* glXGetCurrentDisplay(void);
#endif

int bind_tex(const char* filename, int texnum
#ifdef __cplusplus
=0
#endif
);

void free_tex(int texnum);

#ifdef __cplusplus
}
#endif

#endif
