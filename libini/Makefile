LIBSRC=ini.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libini.a

MAINSRC=test.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
MAINOUT=test

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(REALMAINOUT) $(LIBOUT)

HEADERS=ini.h

include ../makeinclude.macros

