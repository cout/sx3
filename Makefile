# Top-level Makefile for sx3
# There must be a better way to do this!

all: ctl_ gfx_ gltext_ matrix_ physics_ ini_ utils_ console_ sx3_

gfx_:
	make -C gfx
	make -C gfx install

gltext_:
	make -C gltext
	make -C gltext install

matrix_:
	make -C matrix install

physics_:
	make -C physics
	make -C physics install

sx3_:
	make -C sx3

ini_:
	make -C libini
	make -C libini install

utils_:
	make -C utils
	make -C utils install

console_:
	make -C console
	make -C console install

ctl_:
	make -C ctl
	make -C ctl install

clean:
	make -C gfx clean
	make -C gltext clean
	make -C physics clean
	make -C sx3 clean

dep:
	touch sx3/Makefile.d
	touch gfx/Makefile.d
	touch physics/Makefile.d
	touch gltext/Makefile.d
	make -C gfx dep
	make -C gltext dep
	make -C physics dep
	make -C sx3 dep
