LIBSRC= \
    models.c \
	md2.c \
    obj.c \
	textures.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libgfx.a

MAINSRC=main.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=test

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=md2.h models.h textures.h

CFLAGS+=$(GL_CFLAGS) $(SDL_CFLAGS)
LIBS+=-lm $(GL_LIBS) $(SDL_LIBS)
LDFLAGS+=$(GL_LDFLAGS) $(SDL_LDFLAGS)

include ../makeinclude.macros

