LIBSRC=sx3_console.c 
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libsx3_console.a

MAINSRC=
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(REALMAINOUT) $(LIBOUT)

HEADERS=sx3_console.h sx3_console_commands.h

LIBS+=-lm

include ../makeinclude.macros

