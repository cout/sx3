#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include "gltext.h"

#define WIDTH 640
#define HEIGHT 480

static int number = 0;

void wire_text() {
	gltContext *g = gltNewContext();
	int j;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gltFontSize(g, 10, 10);

	for(j = 0; j < 128; j++) {
		gltWireChar(g, j);
		if(j % 40 == 19) gltWireString(g, "\r\n");
	}
	gltWireString(g, "\r\n\r\n");
	gltWireString(g, "The brown fox jumps over the lazy DOG!?!\r\n");
	gltWireString(g, "Now is the time for all good men\r\n");
	gltWireString(g, "to come to the aid of their country.\r\n");

	gltFreeContext(g);
}

static void bitmap_text(int f) {
	int j;
	gltContext *g = gltNewContext();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gltSelectFont(g, f);
	for(j = 0; j < 128; j++) {
		gltBitmapChar(g, j);
		if(j % 40 == 19) gltBitmapString(g, "\r\n");
	}
	gltBitmapString(g, "\r\n\r\n");
	gltBitmapString(g, "The brown fox jumps over the lazy DOG!?!\r\n");
	gltBitmapString(g, "Now is the time for all good men\r\n");
	gltBitmapString(g, "to come to the aid of their country.\r\n");

	gltFreeContext(g);
}

static void display(void) {
	switch(number) {
		case 0: wire_text(); break;
		case 1: bitmap_text(GLT_HELVETICA); break;
		case 2: bitmap_text(GLT_TIMES); break;
		case 3: bitmap_text(GLT_COURIER); break;
		case 4: bitmap_text(GLT_FIXED); break;
	}
	SDL_GL_SwapBuffers();
}

void main_loop() {
	SDL_Event event;

	display();
	for(;;) {
		if(!SDL_WaitEvent(&event)) continue;
		switch(event.type) {
			case SDL_KEYUP:
				number++;
				number %= 5;
				display();
				break;
			case SDL_QUIT:
				return;
		}
	}
}

int main(int argc, char *argv[]) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to initialize SDL\n");
		exit(1);
	}

	atexit(SDL_Quit);

    if(SDL_SetVideoMode(512, 512, 16, SDL_OPENGL) == NULL) {
       fprintf(stderr, "Unable set set GL mode\n");
       exit(1);
	}

	SDL_WM_SetCaption("test", NULL);
	main_loop();

	return 0;
}
