LIBSRC=sx3_registry.c sx3_utils.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libsx3_utils.a

MAINSRC=
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(REALMAINOUT) $(LIBOUT)

HEADERS=sx3_registry.h sx3_utils.h

INCLUDES=-I. -I../local/include

CFLAGS=-g -Wall
CCFLAGS=$(CFLAGS)

LIBS=-lm
LDFLAGS=

include ../makeinclude.macros

install: libinstall
