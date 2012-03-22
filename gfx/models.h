#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>

enum Model_return_values {
    MODEL_OK,
    MODELERR_OPEN,
    MODELERR_UNSUPPORTED,
    MODELERR_VERSION,
    MODELERR_MAGIC,
    MODELERR_RANGE,
    MODELERR_VERTEX,
    MODELERR_FACE,
    MODELERR_NORMALS,
    MODELERR_DISPLAYLIST,
    MODELERR_READ_ERROR,
    MODELERR_UNKNOWN
};

typedef struct {
    int skin;
    int framestart;
    int numframes;
} model_t;

int parse_model(const char *file, const char *skin, model_t *model);
int parse_model_fp(FILE *fp, const char *skin, model_t *model);

#endif
