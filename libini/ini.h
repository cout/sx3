#ifndef INI_H
#define INI_H

#ifndef LIBINI
typedef void INI_Context;
#endif

enum ini_return_values {
    INI_OK,
    INIERR_OPEN
};

INI_Context* ini_new_context();
void ini_free_context(INI_Context *context);
int ini_load_config_file(
    INI_Context *ini,
    const char *filename);
const char *ini_get_value(
    INI_Context *ini,
    const char *section,
    const char *var);
    

#endif
