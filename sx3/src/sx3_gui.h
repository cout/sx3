// File: sx3_gui.h
// AUthor: Marc Bryant
// 
// Header file for the graphical user interface module.

#ifndef SX3_GUI_H
#define SX3_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sx3.h"

// ===========================================================================
// Data types
// ===========================================================================

enum sx3_font_types {
    SX3_DEFAULT_FONT,
    SX3_LAST_FONT
};


// ===========================================================================
// FUnction declarations
// ===========================================================================

SX3_ERROR_CODE sx3_display_text(int x, int y, char* s, enum sx3_font_types font);
SX3_ERROR_CODE sx3_draw_text(int x, int y, char* s, enum sx3_font_types font);
void sx3_draw_hud(float dt);
void sx3_init_gui();
void sx3_close_gui();

#ifdef __cplusplus
}
#endif
#endif
