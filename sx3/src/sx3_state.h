// File: sx3_state.h
// Author: Paul Brannan

#ifndef SX3_STATE_H
#define SX3_STATE_H

enum Game_Mode {
    SX3_TITLE_SCREEN_IN,            // Fade in the title screen
    SX3_TITLE_SCREEN,               // Display the title screen
    SX3_TITLE_SCREEN_OUT,           // Fade out the title screen
    SX3_TITLE_SCREEN_DONE,          // Cleaning up the title screen
    SX3_MAIN_MENU,                  // Main menu
    SX3_SHOP_MENU,                  // User buys items, tanks, etc
    SX3_OPTIONS_MENU,               // Set options for sx3
    SX3_CREDITS,                    // Display credits
    SX3_GAME,                       // Play the game (game loop)!
    SX3_GAME_ANIMATE,               // Firing sequence
    SX3_GAME_END,                   // End the game

    SX3_NUM_GAME_MODES
};

int get_game_mode();
void set_game_mode(int);

#endif
