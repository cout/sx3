PREFIX=../local
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/man
LIBDIR  = $(PREFIX)/lib
INCDIR  = $(PREFIX)/include
SECTION = 1

LIBSRC= \
	models/md2/md2.c \
	textures/textures.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libgfx.a

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

HEADERS=include/md2.h include/models.h include/textures.h

INCLUDES=-Iinclude -I/usr/local/include

ifeq ($(COMSPEC),)
	CFLAGS= -g -O2 -Wall `sdl-config --cflags`
	LIBS=-lGLU -lGL -lm `sdl-config --libs` -lSDL_image
	LDFLAGS=-L/usr/lib32 -L/usr/local/lib32 -L/usr/X11R6/lib
else
	CFLAGS=-g -O2 -Wall -DWIN32
	LIBS=-lopengl32 -lglu32 -lm -lSDL -lSDL_image -lSDLmain
	LDFLAGS=
endif
CC=gcc
AR=ar

#### RULES ####

.SUFFIXES: .c

default: dep $(OUT)

include Makefile.d

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(MAINOUT): $(MAINOBJ)
	$(CC) $(LDFLAGS) $(MAINOBJ) $(LIBS) -o $(MAINOUT)

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
	rm -f $(OBJ) $(OUT) core
