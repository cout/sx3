LIBSRC=ini.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libini.a

MAINSRC=test.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=test

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=ini.h

INCLUDES=-I. -I../local/include

CFLAGS=-g -Wall

LIBS=
LDFLAGS=

include ../build/makeinclude.macros
