#ifdef __MINGW32__
#include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_endian.h>
#include <assert.h>
#include "md2.h"
#include "textures.h"

#define NUMVERTEXNORMALS 162
static float avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};

#define MD2_MAGIC        844121161
#define MD2_VERSION      8

#define MAX_TRIANGLES    4096 
#define MAX_VERTICES     2048 
#define MAX_TEXCOORDS    2048 
#define MAX_FRAMES       512 
#define MAX_SKINS        32 

#ifndef byte
#define byte unsigned char
#endif

/* We don't want to have problems with alignment in the structs */
#pragma pack(1)

typedef struct { 
    int magic;
    int version; 
    int skinWidth; 
    int skinHeight; 
    int frameSize; 
    unsigned int numSkins; 
    unsigned int numVertices; 
    unsigned int numTexCoords; 
    unsigned int numTriangles; 
    unsigned int numGlCommands; 
    unsigned int numFrames; 
    int offsetSkins; 
    int offsetTexCoords; 
    int offsetTriangles; 
    int offsetFrames; 
    int offsetGlCommands; 
    int offsetEnd; 
} md2_t;

typedef struct {
    byte vertex[3];
    byte lightNormalIndex;
} triangleVertex_t;

typedef struct {
    float scale[3];
    float translate[3];
    char name[16];
    triangleVertex_t vertices[1];
} frame_t;

typedef struct {
    short vertexIndices[3];
    short textureIndices[3];
} triangle_t;

typedef struct {
    short s, t;
} textureCoordinate_t;

typedef struct {
    float s, t;
    unsigned int vertexIndex;
} glCommandVertex_t;

int parse_md2(const char *file, const char *skin, model_t *md2) {
    FILE *fp;
    int retcode;

    /* Open the file */
    if((fp = fopen(file, "rb")) == NULL)
        return MODELERR_OPEN;
    
    retcode = parse_md2_fp(fp, skin, md2);
    fclose(fp);
    return retcode;
}

int parse_md2_fp(FILE *fp, const char *skin, model_t *md2) {
    static md2_t                model;
    static glCommandVertex_t    glCommandVertex[MAX_VERTICES];
    static frame_t              frames[MAX_FRAMES];
    static char                 skin_filename[64];
    int                         texnum;
    int                         framesize, i, j, n;
    int                         retcode = MODEL_OK;

    /* Get the header */
    fread(&model, sizeof(md2_t), 1, fp);

    /* Fix the endianness of the header */
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        model.magic = SDL_Swap32(model.magic);
        model.version = SDL_Swap32(model.version);
        model.skinWidth = SDL_Swap32(model.skinWidth);
        model.skinHeight = SDL_Swap32(model.skinHeight);
        model.frameSize = SDL_Swap32(model.frameSize);
        model.numSkins = SDL_Swap32(model.numSkins);
        model.numVertices = SDL_Swap32(model.numVertices);
        model.numTexCoords = SDL_Swap32(model.numTexCoords);
        model.numTriangles = SDL_Swap32(model.numTriangles);
        model.numGlCommands = SDL_Swap32(model.numGlCommands);
        model.numFrames = SDL_Swap32(model.numFrames);
        model.offsetSkins = SDL_Swap32(model.offsetSkins);
        model.offsetTexCoords = SDL_Swap32(model.offsetTexCoords);
        model.offsetTriangles = SDL_Swap32(model.offsetTriangles);
        model.offsetFrames = SDL_Swap32(model.offsetFrames);
        model.offsetGlCommands = SDL_Swap32(model.offsetGlCommands);
        model.offsetEnd = SDL_Swap32(model.offsetEnd);
    }

    /* Do some sanity checking */
    if(model.magic != 0x32504449) return MODELERR_MAGIC;
    if(model.version != MD2_VERSION)
        return MODELERR_VERSION;
    if(model.numSkins > MAX_SKINS ||
       model.numVertices > MAX_VERTICES ||
       model.numTexCoords > MAX_TEXCOORDS ||
       model.numTriangles > MAX_TRIANGLES ||
       model.numGlCommands > MAX_VERTICES ||
       model.numFrames > MAX_FRAMES)
        return MODELERR_RANGE;

    /* Read the first skin */
    /* I'm not yet sure how to use multiple skins */
    if(*skin == 0) {
        fseek(fp, model.offsetSkins, SEEK_SET);
        fread(skin_filename, sizeof(skin_filename), 1, fp);
        /* Load the texture into memory, if necessary */
        texnum = bind_tex(skin_filename, 0);
        md2->skin = texnum;
    } else {
        texnum = bind_tex(skin, 0);
        md2->skin = texnum;
    }

    if((md2->framestart = glGenLists(model.numFrames)) == 0)
        return MODELERR_DISPLAYLIST;
    md2->numframes = model.numFrames;

    /* Read the first frame */
    /* We should read this into an array of frames, optimally */
    framesize = model.frameSize * model.numFrames;
    fseek(fp, model.offsetFrames, SEEK_SET);
    fread(&frames[0], framesize, 1, fp);

    /* Draw the first frame */
    i = 0;
    fseek(fp, model.offsetGlCommands, SEEK_SET);

    for(i = 0; i < md2->numframes; i++) {
        for(j = 0; j < 3; j++) {
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                frames[i].scale[j] = SDL_Swap32(frames[i].scale[j]);
                frames[i].translate[j] = SDL_Swap32(frames[i].translate[j]);
            }
        }

        /* Now create the display list */
        glNewList(md2->framestart + i, GL_COMPILE);

        /* FIX ME!! We should probably do these rotations ourselves, so
         * that we have more stack space to play with */
        glPushMatrix();
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glRotatef(90.0, 0.0, 0.0, -1.0);

        /* FIX ME!! This is a hack to get CW models to work */
        glPushAttrib(GL_POLYGON_BIT);
        glFrontFace(GL_CW);

        if(md2->skin) glBindTexture(GL_TEXTURE_2D, md2->skin);

        do {
            int loc;
            glCommandVertex_t *glvertex;
            triangleVertex_t *tvertex;
            float vertex[3];
            
            /* Read the number of GL Commands we are to process */
            fread(&n, sizeof(int), 1, fp);
            if(feof(fp)) break;
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) n = SDL_Swap32(n);
            
            if(n > 0) {
                glBegin(GL_TRIANGLE_STRIP);
            } else {
                glBegin(GL_TRIANGLE_FAN);
                n = -n;
            }
            
            loc=ftell(fp);
            fread(glCommandVertex, n * sizeof(glCommandVertex_t), 1, fp);
            if(feof(fp)) break;

            for(j = 0; j < n; j++) {
                glvertex = &glCommandVertex[j];
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                    glvertex->s = SDL_Swap32(glvertex->s);
                    glvertex->t = SDL_Swap32(glvertex->t);
                    glvertex->vertexIndex = SDL_Swap32(glvertex->vertexIndex);
                }

                if(glvertex->vertexIndex >= model.numVertices) {
                    retcode = MODELERR_VERTEX;
                    continue;
                }
                
                tvertex = &frames[i].vertices[glvertex->vertexIndex];
                if(tvertex->lightNormalIndex >= NUMVERTEXNORMALS) {
                    retcode = MODELERR_NORMALS;
                    continue;
                }
                
                glNormal3fv(avertexnormals[tvertex->lightNormalIndex]);
                glTexCoord2f(glvertex->s, glvertex->t);
                
                vertex[0] = tvertex->vertex[0] * frames[i].scale[0]
                            + frames[i].translate[0];
                vertex[1] = tvertex->vertex[1] * frames[i].scale[1]
                            + frames[i].translate[1];
                vertex[2] = tvertex->vertex[2] * frames[i].scale[2]
                            + frames[i].translate[2];
                glVertex3fv(vertex);
            }
            
            glEnd();
        } while(n != 0);

        /* FIX ME!! This is a hack to get CW models to work */
        glPopAttrib();

        glPopMatrix();

        glEndList();
    }

    return retcode;
}
