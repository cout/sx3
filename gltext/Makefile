LIBSRC=gltext.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libgltext.a

MAINSRC=test.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=test

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=gltext.h

INCLUDES=-I. -I../local/include

CFLAGS=-g -Wall `sdl-config --cflags`
CC=gcc
AR=ar

LIBS=-lm -lGL -lGLU
LDFLAGS=-L/usr/X11R6/lib `sdl-config --libs`

include ../build/makeinclude.macros

install: libinstall
