PREFIX=../local
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/man
LIBDIR  = $(PREFIX)/lib
INCDIR  = $(PREFIX)/include
SECTION = 1

LIBSRC=pglobal.c physics.c zeroin.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libphysics.a

MAINSRC=main.c
MAINOBJ=$(MAINSRC:.c=.o) $(LIBOBJ)
ifeq ($(COMSPEC),)
	MAINOUT=test
else
	MAINOUT=test.exe
endif

SRC=$(MAINSRC) $(LIBSRC)
OBJ=$(MAINOBJ) $(LIBOBJ)
OUT=$(MAINOUT) $(LIBOUT)

HEADERS=pglobal.h physics.h

INCLUDES=-I. -I../local/include

CFLAGS=-g -Wall
CCFLAGS=$(CFLAGS)
CC=gcc
AR=ar

LIBS=-lm
LDFLAGS=

#### RULES ####

.SUFFIXES: .c

default: dep $(OUT)

include Makefile.d

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(MAINOUT): $(MAINOBJ)
	$(CC) $(MAINOBJ) $(LDFLAGS) $(LIBS) -o $(MAINOUT)

$(LIBOUT): $(LIBOBJ)
	$(AR) rc $@ $?

install:
	mkdir -p $(LIBDIR)
	cp $(LIBOUT) $(LIBDIR)
	mkdir -p $(INCDIR)
	cp $(HEADERS) $(INCDIR)

depend: dep

dep:
	gcc -M $(CFLAGS) $(INCLUDES) $(SRC) > Makefile.d

clean:
	rm -f $(OBJ) $(OUT) core *.exe
