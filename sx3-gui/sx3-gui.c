#include <stdlib.h>
#include "sx3-gui.h"
#include <ctl/c_algo.h>

// ---------------------------------------------------------------------------
// Sx3_Widget_List
// ---------------------------------------------------------------------------

void init_sx3_widget_list(Sx3_Widget_List *wl) {
    DYNARRAY_INIT(*wl);
}

void free_sx3_widget_list(Sx3_Widget_List *wl) {
    DYNARRAY_FREE(*wl);
}

Sx3_Widget_List* sx3_widget_list_add(Sx3_Widget_List *wl, Sx3_Widget *w) {
    DYNARRAY_PUSH_BACK(*wl, w);
}

void sx3_widget_list_remove(Sx3_Widget_List *wl, Sx3_Widget *w) {
    // TODO
    /* DYNARRAY_DELETE(*wl, w); */
}

// ---------------------------------------------------------------------------
// Sx3_Widget_Lookup
// ---------------------------------------------------------------------------

void init_sx3_widget_lookup(Sx3_Widget_Lookup *wup, size_t size) {
    size_t j;
    
    wup->wl = malloc(size * sizeof(*wup->wl));
    wup->size = size;

    for(j = 0; j < wup->size; ++j) {
        init_sx3_widget_list(&wup->wl[j]);
    }
}

void free_sx3_widget_lookup(Sx3_Widget_Lookup *wup) {
    int j;

    for(j = 0; j < wup->size; ++j) {
        free_sx3_widget_list(&wup->wl[j]);
    }
    free(wup->wl);
}

void sx3_widget_lookup_add(
        Sx3_Widget_Lookup *wup,
        Widget *w,
        size_t min,
        size_t max) {

    size_t j;

    for(j = min; j <= max; ++j) {
        sx3_widget_list_add(&wup->wl[j], w)
    }
}

void sx3_widget_lookup_remove(
        Sx3_Widget_Lookup *wup,
        Widget *w,
        size_t min,
        size_t max) {

    size_t j;

    for(j = min; j <= max; ++j) {
        sx3_widget_list_remove(&wup->wl[j], w)
    }
}

// ---------------------------------------------------------------------------
// Sx3_Gui
// ---------------------------------------------------------------------------

Sx3_Gui* new_sx3_gui(size_t width, size_t height) {
    Sx3_Gui *gui = malloc(sizeof(Sx3_Gui));
    init_sx3_widget_lookup(gui->lookup[0], width);
    init_sx3_widget_lookup(gui->lookup[1], height);
    DYNARRAY_INIT(gui->widgets);
    gui->idle_func = NULL;
    return gui;
}

void free_sx3_gui(Sx3_Gui *gui) {
    free_sx3_widget_list(gui->lookup[0]);
    free_sx3_widget_list(gui->lookup[1]);
    free(gui);
}

void sx3_gui_set_max_events(Sx3_Gui *gui, int max_events) {
    gui->max_events = max_events;
}

void sx3_gui_main_loop(Sx3_Gui *gui) {
    Uint32 old_time = SDL_GetTicks(), time;
    float dt;
    SDL_Event event;
    int count;

    for(count = 0; count < gui->max_Events && SDL_PollEvent(&event); count++) {
        switch(event.type) {
            case SDL_KEYDOWN:
                // TODO
                break;
            case SDL_KEYUP:
                // TODO
                break;
            case SDL_MOUSEMOTION:
                // TODO
                break;
            case SDL_MOUSEBUTTONDOWN:
                // TODO
                break;
            case SDL_MOUSEBUTTONUP:
                // TODO
                break;
            case SDL_QUIT:
                return;
            default:
                break;
        }

        time = SDL_GetTicks();
        dt = (float)(time - old_time) / 1000.0;
        old_time = time;

        gui->idle_func(dt, gui->idle_arg);
    }
}

void sx3_gui_draw(Sx3_Gui *gui) {
    FOR_EACH(
        DYNARRAY,
        Sx3_Widget *,
        DYNARRAY_BEGIN(gui->widgets),
        DYNARRAY_END(gui->widgets),
        sx3_widget_draw);
}

void sx3_gui_set_idle_function(Sx3_Gui *gui, Sx3_Idle_Function idle_func) {
    gui->idle_func = idle_func;
}

// ---------------------------------------------------------------------------
// Sx3_Widget
// ---------------------------------------------------------------------------

Sx3_Widget* new_sx3_widget(Sx3_Gui *gui) {
    Sx3_Widget *w = malloc(sizeof(Sx3_Widget));
    w->x = 0;
    w->y = 0;
    w->width = 0;
    w->height = 0;
    w->attr = SX3_WIDGET_DEFAULT_ATTRIBUTES;
    w->draw = NULL;
    w->mouse_button_cb = NULL;
    w->mouse_motion_cb = NULL;
    w->keyboard_cb = NULL;
    return w;
}

void register_sx3_widget(Sx3_Gui *gui, Sx3_Widget *w) {
    sx3_widget_lookup_add(gui->lookup[0], w, w->x, w->x + w->width);
    sx3_widget_lookup_add(gui->lookup[1], w, w->y, w->y + w->height);
    DYNARRAY_PUSH_BACK(gui->widgets, w);
}

void free_sx3_widget(Sx3_Gui *gui, Sx3_Widget *w) {
    sx3_widget_lookup_remove(gui->lookup[0], w, w->x, w->x + w->width);
    sx3_widget_lookup_remove(gui->lookup[1], w, w->y, w->y + w->height);
    // TODO
    /* DYNARRAY_DELETE(gui->widgets, w); */
    free(widget);
}

void sx3_widget_draw(Sx3_Widget *w) {
    if(w->draw) w->draw(w);
}
