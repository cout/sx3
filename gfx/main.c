#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_main.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <models.h>

/* Useful defines */
#define ZOOM          ((unsigned int)1)
#define ROTATE        ((unsigned int)2)

#define ZOOM_INC      0.25
#define ROTATE_INC    0.25

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE !FALSE
#endif

/* Menu options */
enum menu_options {
    MENU_TEXTURE,
    MENU_LIGHTING,
    MENU_WIREFRAME,
    MENU_LINEAR,
    MENU_AXES,
    MENU_WEAPON,
    MENU_ANIMATE,
    MENU_ANIM_FORWARD,
    MENU_ANIM_REVERSE,
    MENU_ANIM_PLUS,
    MENU_ANIM_MINUS,
    MENU_ANIM_REWIND,
    MENU_ANIM_STOP,
    MENU_EXIT
};

int texture=TRUE, lighting=TRUE, axes=TRUE, weapon=TRUE,
    wireframe=FALSE, filtering=TRUE;

/* Global variables */
int              action;
float            eye[3] = {0.0, 0.0, 100.0};
float            view[3] = {0.0, 0.0, 0.0};

float            theta = 0.0, theta0 = 0.0;
float            phi = 0.0, phi0 = 0.0;
float            z = 100.0, z0 = 100.0;

model_t          model;
model_t          weapon_model;
int              mousex, mousey, x0, y0;

/* Animation globals */
int              currentFrame = 0;
int              frameDirection = 0;

// Set the projection matrix to match the width and height of the window.
void set_window_size(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, height);
}

// This is the display function.  Note the use of SDL_GL_SwapBuffers(),
// below.
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], view[0], view[1], view[2], 0.0, 1.0, 0.0);

    glRotatef(theta, 0.0, 1.0, 0.0);
    glRotatef(phi, 1.0, 0.0, 0.0);
    
    // Draw coordinate axes
    glColor3f(1.0, 1.0, 1.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    if(axes) {
        glBegin(GL_LINES);
            glVertex3f(-100.0, 0.0, 0.0);
            glVertex3f(100.0, 0.0, 0.0);
            glVertex3f(0.0, -100.0, 0.0);
            glVertex3f(0.0, 100.0, 0.0);
            glVertex3f(0.0, 0.0, -100.0);
            glVertex3f(0.0, 0.0, 100.0);
        glEnd();
    }
    if(lighting) glEnable(GL_LIGHTING);
    if(texture) glEnable(GL_TEXTURE_2D);

    // Draw the model
    currentFrame %= model.numframes;
    glCallList(model.framestart + currentFrame);

    if(weapon) glCallList(weapon_model.framestart);

    // And swap buffers to display the image
    SDL_GL_SwapBuffers();
}

// A handler for mouse button events.  These are not the proper types for
// the parameters, but we will let C handle the conversions.
void mouse_button(int button, int state, int x, int y) {
    if(state==SDL_PRESSED) {
        x0 = x; y0 = y; theta0 = theta; phi0 = phi; z0 = z;
        if(button&SDL_BUTTON_LMASK) action |= ZOOM;
        if(button&SDL_BUTTON_MMASK) action |= ROTATE;
    } else if(state==SDL_RELEASED) {
        action &= ~ZOOM;
        action &= ~ROTATE;
    }
}

// A handler for mouse motion events.  Again, the parameter types are not
// quite right.
void mouse_motion(int state, int x, int y) {
    mousex = x; mousey = y;
    if(action & ZOOM) {
        z = z0 + ZOOM_INC * (mousey - y0);
        eye[2] = z;
        theta = theta0 + ROTATE_INC * (mousex - x0);
        display();
    } else if(action & ROTATE) {
        theta = theta0 + ROTATE_INC * (mousex - x0);
        phi = phi0 + ROTATE_INC * (mousey - y0);
        display();
    }
}

// Animate the image by advancing the frame count.
void animate(int value) {
    if(frameDirection) {
        currentFrame += frameDirection;
        display();
    }
}

void gl_settings() {
    if(texture) {
        glEnable(GL_TEXTURE_2D);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    if(wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
    }

    if(filtering) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        // For some reason, setting the MIN filter will cause some video
        // cards to crash on Win32.
#ifndef WIN32
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR);
#endif
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);
        // For some reason, setting the MIN filter will cause some video
        // cards to crash on Win32.
#ifndef WIN32
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_NEAREST);
#endif
    }
}


// This was originally a GLUT function for capturing menu events.  It
// no longer serves that function, but instead acts as a multi-purpose
// function for setting display options.
void menu_func(int choice) {
    switch(choice) {
    case MENU_TEXTURE:
        texture = !texture;
        gl_settings();
        display();
        break;
    case MENU_LIGHTING:
        lighting = !lighting;
        gl_settings();
        display();
        break;
    case MENU_WIREFRAME:
        wireframe = !wireframe;
        gl_settings();
        display();
        break;
    case MENU_LINEAR:
        filtering = !filtering;
        gl_settings();
        display();
        break;
    case MENU_AXES:
        axes = !axes;
        gl_settings();
        display();
        break;
    case MENU_WEAPON:
        weapon = !weapon;
        gl_settings();
        display();
        break;
    case MENU_ANIM_FORWARD: frameDirection = 1; break;
    case MENU_ANIM_REVERSE: frameDirection = -1; break;
    case MENU_ANIM_PLUS: currentFrame++; display(); break;
    case MENU_ANIM_MINUS: currentFrame--; display(); break;
    case MENU_ANIM_REWIND: currentFrame = 0;
    case MENU_ANIM_STOP: frameDirection = 0; display(); break;
    case MENU_EXIT: exit(0); break;
    }
}

// A handler for key events.  Note that once again the parameter type is
// wrong.  In this case, all normal ASCII keys do map properly, but some
// special characters are also available for our use.  See the SDL
// documentation for more details.
void keyfunc(unsigned char key) {
    switch(tolower(key)) {
    case 't': menu_func(MENU_TEXTURE); break;
    case 'l': menu_func(MENU_LIGHTING); break;
    case 'x': menu_func(MENU_AXES); break;
    case 'w': menu_func(MENU_WEAPON); break;
    case 'r': menu_func(MENU_WIREFRAME); break;
    case 'f': menu_func(MENU_LINEAR); break;
    case 'a': menu_func(MENU_ANIM_FORWARD); break;
    case 'b': menu_func(MENU_ANIM_REVERSE); break;
    case 's': menu_func(MENU_ANIM_STOP); break;
    case '<':
    case ',': menu_func(MENU_ANIM_MINUS); break;
    case '>':
    case '.': menu_func(MENU_ANIM_PLUS); break;
    case 'e': menu_func(MENU_ANIM_REWIND); break;
    case '+': z += ZOOM_INC; eye[2] = z; display(); break;
    case '-': z -= ZOOM_INC; eye[2] = z; display(); break;
    case '8': phi += ROTATE_INC; display(); break;
    case '2': phi -= ROTATE_INC; display(); break;
    case '4': theta -= ROTATE_INC; display(); break;
    case '6': theta += ROTATE_INC; display(); break;
    case 'q': exit(0); break;
    }
}

// Initialize OpenGL.
void init(const char *modelfile, const char *weaponfile, const char *skinfile) {
    int retval;

    GLfloat light0_diffuse[] = {2.0, 2.0, 2.0, 1.0};
    GLfloat light0_specular[] = {2.0, 2.0, 2.0, 1.0};
    GLfloat light0_position[] = {0.0, 0.0, 50.0, 0.0};
    GLfloat light0_ambient[] = {0.0, 0.0, 0.0, 1.0};

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_SMOOTH);
    // glEnable(GL_POLYGON_SMOOTH);

    // Create a light source.
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // Enable lighting, and enable the light source
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    // Parse the model files and put the frames into a display list
    retval = parse_model(modelfile, skinfile, &model);
    assert(retval != MODELERR_OPEN);
    assert(retval != MODELERR_VERSION);
    assert(retval != MODELERR_MAGIC);
    assert(retval != MODELERR_RANGE);
    assert(retval != MODELERR_VERTEX);
    assert(retval != MODELERR_NORMALS);
    assert(retval != MODELERR_DISPLAYLIST);
    assert(retval == MODEL_OK);
    retval = parse_model(weaponfile, "", &weapon_model);

    gl_settings();
}

// SDL doesn't use callbacks, so we must check the events ourselves
void main_loop() {
    SDL_Event event;
    SDL_Event prev_event;
    int retval;

    for(;;) {
        // We can wait on an event (as we are doing here), or we can poll for
        // events and, if there are no events waiting to be processed, we can
        // call our idle function.  This allows for a slightly more versatile
        // event handler than with a callback-based approach, as with GLUT.
        if(!SDL_WaitEvent(&event)) continue;
event_switch:
        switch(event.type) {
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouse_button(event.button.button, event.button.state,
                    event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                // This is a hack to keep the motion from being so jumpy on
                // slow cards
                do {
                    prev_event = event;
                    retval = SDL_PollEvent(&event);
                } while(retval != 0 && event.type == SDL_MOUSEMOTION);
                if(retval != 0) {
                    // SDL_PushEvent(&event);
                    // event = prev_event;
                    mouse_motion(prev_event.motion.state,
                        prev_event.motion.x, prev_event.motion.y);
                    goto event_switch;
                }
                mouse_motion(event.motion.state,
                    event.motion.x, event.motion.y);
                break;
            case SDL_KEYUP:
                keyfunc(event.key.keysym.sym);
                break;
            case SDL_VIDEORESIZE:
                set_window_size(event.resize.w, event.resize.h);
                break;
            case SDL_QUIT:
                return;
        }
    }
}

// Due to a bug in SDL, we have to declare WinMain here.  I'm not sure why,
// since it should already be defined for us.
#ifdef __MINGW32__
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    return main(_argc, _argv);
}
#endif

int main(int argc, char *argv[]) {
    // Check for command-line arguments
    if(argc < 3) {
        fprintf(stderr, "Usage: %s: [options] model weapon skin\n",
            argv[0]);
        exit(1);
    }

    // Intialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to initialize SDL\n");
        exit(1);
    }

    // Set our quit function so that we clean up properly
    atexit(SDL_Quit);

    // Set our desired GL attributes.  I'm not sure if this is necessary,
    // and I'm not sure what happens if the desired attributes cannot be
    // negotiated.
/*
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
*/
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if(SDL_SetVideoMode(640, 480, 16, SDL_OPENGL) == NULL) {
        fprintf(stderr, "Unable set set GL mode\n");
        exit(1);
    }

    // Set the title of the Window, and use the default icon.  We could
    // specify an icon for the window, if we had one.
    SDL_WM_SetCaption("Model Viewer", NULL);

    // Initialize the viewer
    init(argv[1], argv[2], argv[3]);

    // Display the model for the first time
    set_window_size(640, 480);
    display();

    // Go into the event loop.  Unlike with GTK or GLUT, this function *will*
    // return.
    main_loop();

    return 0;
}
