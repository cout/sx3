PREFIX=../local
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/man
LIBDIR  = $(PREFIX)/lib
INCDIR  = $(PREFIX)/include
SECTION = 1

LIBSRC=ini.c
LIBOBJ=$(LIBSRC:.c=.o)
LIBOUT=libini.a

TESTSRC=test.c
TESTOBJ=$(TESTSRC:.c=.o) $(LIBOBJ)
ifeq ($(COMSPEC),)
	TESTOUT=test
else
	TESTOUT=test.exe
endif

SRC=$(TESTSRC) $(LIBSRC)
OBJ=$(TESTOBJ) $(LIBOBJ)
OUT=$(TESTOUT) $(LIBOUT)

HEADERS=ini.h

INCLUDES=-I. -I../local/include

CFLAGS=-g -Wall -fno-exceptions
CC=gcc
AR=ar

LIBS=
LDFLAGS=

#### RULES ####

.SUFFIXES: .c

default: dep $(OUT)

include Makefile.d

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(TESTOUT): $(TESTOBJ)
	$(CC) $(TESTOBJ) $(LDFLAGS) $(LIBS) -o $(TESTOUT)

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
