#ifndef SX3_GUI_H
#define SX3_GUI_H

#include <ctl/c_dynarray.h>

typedef void (*Sx3_Draw_Function)(Sx3_Widget * w);
typedef void (*Sx3_Keyboard_Function)();
typedef void (*Sx3_Mouse_Button_Function)();
typedef void (*Sx3_Mouse_Motion_Function)();
typedef void (*Sx3_Idle_Function)(float dt, void * arg);

#define SX3_WIDGET_DEFAULT_ATTRIBUTES   0
#define SX3_WIDGET_ATTR_VISIBLE         1 << 0
#define SX3_WIDGET_ATTR_ENABLED         1 << 1
#define SX3_WIDGET_ATTR_ACTIVE          1 << 2
#define SX3_WIDGET_ATTR_TOGGLED         1 << 3
#define SX3_WIDGET_ATTR_HIGHLIGHTED     1 << 4

typedef struct Sx3_Widget {
    int x, y;
    int width, height;
    int attr;
    Sx3_Draw_Function draw;
    Sx3_Button_Function mouse_button_cb;
    Sx3_Button_Function mouse_motion_cb;
    Sx3_Keyboard_Function keyboard_cb;
};

typedef DYNARRAY(Sx3_Widget *) Sx3_Widget_List;

typedef struct Sx3_Widget_Lookup {
    Sx3_Widget_List *wl;
    size_t size;
};

typedef struct Sx3_Gui {
    Sx3_Widget_Lookup *lookup[2];
    Sx3_Widget_List widgets;
    Sx3_Idle_Function idle_func;
};

// Sx3_Widget_List functions
void init_sx3_widget_list(Sx3_Widget_List *wl);
void free_sx3_widget_list(Sx3_Widget_List *wl);
Sx3_Widget_List* sx3_widget_list_add(Sx3_Widget_List *wl, Sx3_Widget *w);
void sx3_widget_list_remove(Sx3_Widget_List *wl, Sx3_Widget *w);

// Sx3_Widget_Lookup functions
void init_sx3_widget_lookup(Sx3_Widget_Lookup *wup, size_t size);
void free_sx3_widget_lookup(Sx3_Widget_Lookup *wup);
void sx3_widget_lookup_add(
    Sx3_Widget_Lookup *wup,
    Widget *w,
    size_t min,
    size_t max);
void sx3_widget_lookup_remove(
    Sx3_Widget_Lookup *wup,
    Widget *w,
    size_t min,
    size_t max);

// Sx3_Gui functions
Sx3_Gui* new_sx3_gui(size_t width, size_t height);
void free_sx3_gui(Sx3_Gui *gui);
void sx3_gui_set_max_events(Sx3_Gui *gui, int max_events);
void sx3_gui_main_loop(Sx3_Gui *gui);
void sx3_gui_draw(Sx3_Gui *gui);
void sx3_gui_set_idle_function(Sx3_Gui *gui, Sx3_Idle_Fucntion idle_func);

// Sx3_Widget functions
Sx3_Widget* new_sx3_widget(Sx3_Gui *gui);
void register_sx3_widget(Sx3_Gui *gui, Sx3_Widget *w);
void free_sx3_widget(Sx3_Gui *gui, Sx3_Widget *w);
void sx3_widget_draw(Sx3_Widget *w);

#endif
