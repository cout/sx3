#ifndef MD2_H
#define MD2_H

#include <stdio.h>
#include "models.h"

int parse_md2(const char *file, const char *skin, model_t *model);
int parse_md2_fp(FILE *fp, const char *skin, model_t *model);

#endif
