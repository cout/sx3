/*
** sx3_global.h
**
**   Variables and prototypes needed to access the global variable database.
**
** Author: Marc Bryant
** Copyright (c) 2000
*/

#ifndef _SX3_REGISTRY_H_
#define _SX3_REGISTRY_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GV_SUCCESS,
    GV_ERROR_BUFFER_TOO_SMALL,
    GV_ERROR_CANNOT_OPEN_FILE,
    GV_ERROR_MEM_ALLOC,
    GV_ERROR_BAD_PARAMS,
    GV_ERROR_G_VAR_NOT_FOUND,
    GV_ERROR_G_VAR_WRONG_TYPE,
    GV_ERROR_G_VAR_STRING_TOO_SHORT,
    GV_ERROR_G_VAR_BAD_DATA_TYPE,
    GV_ERROR_G_VAR_ALREADY_EXISTS,
    GV_ERROR_G_VAR_READ_ONLY,

    GV_ERROR_COUNT_,
    GV_ERROR_INVALID_
} GV_ERROR_CODE;


/* GLOBAL VARIABLE TYPES *****************************************************/
enum sx3_global_data_types {
    SX3_GLOBAL_STRING,
    SX3_GLOBAL_INT,    /* int                  */
    SX3_GLOBAL_BOOL,   /* int as well          */
    SX3_GLOBAL_FLOAT,  /* float                */
    SX3_GLOBAL_DOUBLE, /* double               */
    SX3_GLOBAL_CHAR,   /* char                 */
    SX3_GLOBAL_PTR,    /* void*                */
    SX3_GLOBAL_VECTOR, /* float [4]            */
    SX3_GLOBAL_LAST
};


/* GLOBAL VARIABLE ACCESS FUNCTIONS *******************************************
** Note: use these functions to access the global variables when speed is not
** a crucial issue.
*/
GV_ERROR_CODE sx3_add_global_var          (char *name, 
                                            enum sx3_global_data_types type,
                                            int read_only,
                                            void *value,
                                            int str_len,
                                            void (*update_function) (void));
GV_ERROR_CODE sx3_release_global_vars     (void);
GV_ERROR_CODE sx3_print_global_value      (const char *var, char *buffer, int *length);

GV_ERROR_CODE sx3_set_global_value        (const char *var, const char  *value);
GV_ERROR_CODE sx3_set_global_int          (const char *var, const int    value);
GV_ERROR_CODE sx3_set_global_string       (const char *var, const char  *value);
GV_ERROR_CODE sx3_set_global_bool         (const char *var, const int    value);
GV_ERROR_CODE sx3_set_global_float        (const char *var, const float  value);
GV_ERROR_CODE sx3_set_global_double       (const char *var, const double value);
GV_ERROR_CODE sx3_set_global_char         (const char *var, const char   value);

GV_ERROR_CODE sx3_force_set_global_value  (const char *var, const char  *value);
GV_ERROR_CODE sx3_force_set_global_int    (const char *var, const int    value);
GV_ERROR_CODE sx3_force_set_global_string (const char *var, const char  *value);
GV_ERROR_CODE sx3_force_set_global_bool   (const char *var, const int    value);
GV_ERROR_CODE sx3_force_set_global_float  (const char *var, const float  value);
GV_ERROR_CODE sx3_force_set_global_double (const char *var, const double value);
GV_ERROR_CODE sx3_force_set_global_char   (const char *var, const char   value);

int            sx3_get_global_int          (const char *var, GV_ERROR_CODE *error);
int            sx3_get_global_string       (const char *var, char *buffer, int length, GV_ERROR_CODE *error);
int            sx3_get_global_bool         (const char *var, GV_ERROR_CODE *error);
float          sx3_get_global_float        (const char *var, GV_ERROR_CODE *error);
double         sx3_get_global_double       (const char *var, GV_ERROR_CODE *error);
char           sx3_get_global_char         (const char *var, GV_ERROR_CODE *error);
GV_ERROR_CODE sx3_find_global_var          (const char *ss, char *var_name);
const char * sx3_gv_error_lookup 		   (GV_ERROR_CODE error);

GV_ERROR_CODE sx3_read_config_file(const char *filename);

#ifdef __cplusplus
}
#endif
#endif
