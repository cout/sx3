HEADERS= \
	c_algo.h \
	c_dynarray.h \
	c_dynarray.i \
	c_itor.h \
	c_hash.h \
	c_hash.i \
	c_types.h \
	c_util.h \
	ctl_config.h
SRC=$(wildcard *_test.c)

all: dynarray_test itor_test hash_test

%_test: %_test.c

clean: testclean

testclean:
	rm -f *_test *_test.o

foo:
	echo $(DEFAULT_INCDIR)
	echo $(INCDIR)

include ../makeinclude.macros

INCDIR=$(addsuffix /ctl, $(DEFAULT_INCDIR))
