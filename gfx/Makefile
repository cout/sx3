LIBSRC= \
    models/models.c \
	models/md2/md2.c \
    models/obj/obj.c \
	textures/textures.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libgfx.a

MAINSRC=main.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=test

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=include/md2.h include/models.h include/textures.h

INCLUDES=-Iinclude -I/usr/local/include

CFLAGS+=-g -O2 -Wall $(GL_CFLAGS) $(SDL_CFLAGS)
LIBS+=-lm $(GL_LIBS) $(SDL_LIBS)
LDFLAGS+=$(GL_LDFLAGS) $(SDL_LDFLAGS)

include ../build/makeinclude.macros

install: libinstall
