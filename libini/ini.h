#ifndef INI_H
#define INI_H

typedef struct INI_Context_ INI_Context;

enum ini_return_values {
    INI_OK,
    INIERR_OPEN
};

INI_Context* ini_new_context(void);

void ini_free_context(INI_Context *context);

int ini_load_config_file(INI_Context *ini, const char *filename);

const char *ini_get_value(
    INI_Context *ini,
    const char *section,
    const char *var);
    
void ini_split_var_value(
    const char *string,
/* WHAT IS string paul??? i  A ini key? (TM AND SB) */
    char *var, int var_len,
    char *val, int val_len);

#endif
