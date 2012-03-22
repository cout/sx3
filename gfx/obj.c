#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <stdlib.h>
#include "textures.h"
#include "obj.h"

#define MAX_VERTICES    2048
#define MAX_FACES       2048

#ifdef WIN32
// TODO: This is a hack to get this file to compile under windows; it's not thread-safe,
// so we should fix this.
#define STRTOK_R(a,b,c) strtok(a,b)
#else
#define STRTOK_R(a,b,c) strtok_r(a,b,c)
#endif

typedef struct {
    float vertex[MAX_VERTICES][3];
} obj_t;

int parse_obj(const char *file, const char *skin, model_t *model) {
    FILE *fp;
    int retcode;

    // Open the file
    if((fp = fopen(file, "rb")) == NULL)
        return MODELERR_OPEN;
    
    retcode = parse_obj_fp(fp, skin, model);
    fclose(fp);
    return retcode;
}

int parse_obj_fp(FILE *fp, const char *skin, model_t *model) {
    char buf[1024];
    char *cmd, *arg;
    char *endptr;
    obj_t obj;
    int v = 0, f = 0;
    int retcode = MODEL_OK;

    // Allocate a display list
    if((model->framestart = glGenLists(1)) == 0)
        return MODELERR_DISPLAYLIST;
    model->numframes = 1;
    glNewList(model->framestart, GL_COMPILE);

    // A very simple object file loader -- we don't even do textures!
    for(;;) {
        if(fgets(buf, sizeof(buf), fp) == NULL) break;
        if(feof(fp)) break;

        cmd = STRTOK_R(buf, " \t", &endptr);
        arg = STRTOK_R(NULL, " \t", &endptr);

        if(!strcmp(cmd, "v")) {
            if(v < MAX_VERTICES) {
                // Vertex command
                sscanf(arg, "%f %f %f",
                    &obj.vertex[v][0], &obj.vertex[v][1], &obj.vertex[v][2]);
                v++;
            } else {
                retcode = MODELERR_VERTEX;
            }
        } else if(!strcmp(cmd, "f")) {
            // Face command
            if(f < MAX_FACES) {
                glBegin(GL_POLYGON);
                do {
                    int vert = atoi(arg);
                    glVertex3fv(obj.vertex[vert]);
                    arg = STRTOK_R(NULL, " \t", &endptr);
                } while(arg != NULL);
                glEnd();
            } else {
                retcode = MODELERR_FACE;
            }
        }
    }

    glEndList();

    return retcode;
}
