LIBSRC=gltext.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libgltext.a

# MAINSRC=test.c
# MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
# MAINOUT=test
MAINSRC=
MAINOBJ=
MAINOUT=

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=gltext.h

INCLUDES+=-I. -I../local/include

CFLAGS+=-g -Wall $(GL_CFLAGS) $(SDL_CFLAGS)
LIBS+=-lm -lgfx $(GL_LIBS) $(SDL_LIBS)
LDFLAGS+=$(GL_LDFLAGS) $(SDL_LDFLAGS)

include ../makeinclude.macros

install: libinstall
