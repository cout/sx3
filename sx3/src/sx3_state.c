// File: sx3_state.c
// Author: Paul Brannan
//
// Keep track of the game's state between modules

#include "sx3_state.h"

static int game_mode = SX3_TITLE_SCREEN_IN;

int get_game_mode()
{
    return game_mode;
}

void set_game_mode(int n)
{
    game_mode = n;
}
