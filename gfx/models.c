#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "md2.h"
#include "obj.h"

const char* get_extension(const char *file) {
    int j;
    for(j = 0; file[j] != 0 && file[j] != '.'; ++j) ;
    if(file[j] == 0) return 0;
    return &file[j+1];
}

int parse_model(const char *file, const char *skin, model_t *model) {
    const char *ext = get_extension(file);
    int retcode = MODELERR_UNKNOWN;

    if(ext == 0) {
        retcode = MODELERR_UNSUPPORTED;
    } else if(!strcasecmp(ext, "MD2")) {
        retcode = parse_md2(file, skin, model);
    } else if(!strcasecmp(ext, "OBJ")) {
        retcode = parse_obj(file, skin, model);
    }

    return retcode;
}

int parse_model_fp(FILE *fp, const char *skin, model_t *md2) {
    // FIX ME!!  For now, we have now way to determine the file type from
    // file pointer, so we will just return an error code.
    return MODELERR_UNKNOWN;
}

