# Top-level Makefile for sx3
# There must be a better way to do this!

all: ctl_ gfx_ gltext_ matrix_ physics_ ini_ utils_ console_ sx3_

gfx_:
	$(MAKE) -C gfx
	$(MAKE) -C gfx install

gltext_:
	$(MAKE) -C gltext
	$(MAKE) -C gltext install

matrix_:
	$(MAKE) -C matrix install

physics_:
	$(MAKE) -C physics
	$(MAKE) -C physics install

sx3_:
	$(MAKE) -C sx3

ini_:
	$(MAKE) -C libini
	$(MAKE) -C libini install

utils_:
	$(MAKE) -C utils
	$(MAKE) -C utils install

console_:
	$(MAKE) -C console
	$(MAKE) -C console install

ctl_:
	$(MAKE) -C ctl
	$(MAKE) -C ctl install

clean:
	$(MAKE) -C gfx clean
	$(MAKE) -C gltext clean
	$(MAKE) -C physics clean
	$(MAKE) -C sx3 clean

dep:
	touch sx3/Makefile.d
	touch gfx/Makefile.d
	touch physics/Makefile.d
	touch gltext/Makefile.d
	$(MAKE) -C gfx dep
	$(MAKE) -C gltext dep
	$(MAKE) -C physics dep
	$(MAKE) -C sx3 dep
