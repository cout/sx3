LIBSRC=pglobal.c physics.c zeroin.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libphysics.a

MAINSRC=main.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=test

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=pglobal.h physics.h

INCLUDES=-I. -I../local/include

CFLAGS=-g -Wall
CCFLAGS=$(CFLAGS)

LIBS=-lm
LDFLAGS=

include ../build/makeinclude.macros

install: libinstall
