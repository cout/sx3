#ifndef OBJ_H
#define OBJ_H

#include <stdio.h>
#include "models.h"

int parse_obj(const char *file, const char *skin, model_t *model);
int parse_obj_fp(FILE *fp, const char *skin, model_t *model);

#endif
