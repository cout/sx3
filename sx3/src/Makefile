MAINSRC= \
        main.c sx3_engine.c sx3_graphics.c \
        sx3_global.c sx3_gui.c sx3_math.c sx3_misc.c \
        sx3_tanks.c sx3_terrain.c sx3_weapons.c sx3_state.c sx3_game.c \
        sx3_title.c sx3_audio.c
MAINOBJ=$(SRC:.c=.o)
MAINOUT=../sx3

SRC=$(MAINSRC)
OBJ=$(MAINOBJ)
OUT=$(REALMAINOUT)

CFLAGS+=-g -O3 -Wall $(GL_CFLAGS) $(SDL_CFLAGS)
LDFLAGS+=$(GL_LDFLAGS) $(SDL_LDFLAGS)
LIBS+=  \
	-lm -lgfx -lphysics -lini -lgltext -lsx3_utils -lsx3_console \
	$(GL_LIBS) $(SDL_LIBS)
CC=gcc
CCC=CC

include ../../makeinclude.macros

PREFIX=../../local
