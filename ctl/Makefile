HEADERS= \
	c_algo.h \
	c_dynarray.h \
	c_dynarray.i \
	c_itor.h \
	c_hash.h \
	c_hash.i \
	c_types.h \
	c_util.h
SRC=$(wildcard *_test.c)

CFLAGS=-g -Wall
CCFLAGS=$(CFLAGS)

all: dynarray_test itor_test hash_test

%_test: %_test.c

foo:
	echo $(DEFAULT_INCDIR)
	echo $(INCDIR)

include ../makeinclude.macros

install: headerinstall

INCDIR=$(addsuffix /ctl, $(DEFAULT_INCDIR))
