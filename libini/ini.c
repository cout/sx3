#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ctl/c_hash.h>

typedef struct INI_HASH_KEY_{
    const char *section_name;
    const char *key_name;
} INI_Hash_Key;
 
struct INI_Context_ {
    HASH(INI_Hash_Key*, const char*) hashtable; 
};

#define LIBINI
#include "ini.h"

static size_t ini_generate_hash_val(const INI_Hash_Key **key);

struct INI_Context_* ini_new_context(void) {
    struct INI_Context_ *new_context = malloc(sizeof(struct INI_Context_));
    // We're using 29 cause it seemed like a good number, oh, it's prime too :-) 
    HASH_INIT(new_context->hashtable, 29, ini_generate_hash_val);
    return new_context;
}

void ini_free_context(struct INI_Context_ *context) {
    //We are assuming the the program is ending, so we're not going to the
    //trouble of freeing the keys and stuff. Hopefully ctl will handle this soon.
    HASH_FREE(context->hashtable);
    free(context);
}

static size_t ini_generate_hash_val(const INI_Hash_Key **key) {
    //Generating the hash val by multipling the ascii values of the strings.
    size_t key_val = 1;
    const char *ptr = (*key)->section_name;
    while(*ptr != '\0') {
        key_val *= *ptr;
        ++ptr; 
    }
    ptr = (*key)->key_name;
    while(*ptr != '\0') {
        key_val *= *ptr;
        ++ptr; 
    }
    return (key_val);
}

static int key_comp(const INI_Hash_Key **a, const INI_Hash_Key **b) {
    int retval;
    retval = strcmp((*a)->section_name,(*b)->section_name);
    if(retval == 0) retval = strcmp((*a)->key_name,(*b)->key_name);
    return retval;
}

static CTL_BOOL is_blank_line(const char *s) {
    for(; *s != '\0'; ++s) {
        if(!isspace(*s)) return FALSE;
    }
    return TRUE;
}

int ini_load_config_fp(struct INI_Context_ *ini, FILE *fp) {
    //These function reads in the contents of a ini file, line by line. 
    char s[1024];
    char *section_name = 0; //stores the current section name, we only see it
                            //on the line that starts the ini section. 

    while(TRUE) {
        if(fgets(s, sizeof(s), fp) == NULL) break;
        if(feof(fp)) break;

        if(s[0] == '#' || s[0] == ';' || is_blank_line(s)) {

            // Comment, ignoring

        } else if(s[0] == '[') {

            // New section
            char *name = s+1, *ptr = s+1;
            while(*ptr != '\r' && *ptr != '\n' && *ptr != ']' && *ptr != 0)
                ++ptr;
            *ptr = 0;
            section_name = malloc(strlen(name)+1);
            strcpy(section_name,name);

        } else { 

            // New entry
            char name[1024];
            char value[1024];
            char *val, *key;
            INI_Hash_Key *new_key = 0;
            ini_split_var_value(s, name, sizeof(name), value, sizeof(value));

            //Should the below event be recorded anywhere? (TM, SB)
            if(!section_name) continue;

            new_key = malloc(sizeof(INI_Hash_Key));
            new_key->section_name = section_name;
            new_key->key_name = key = malloc(strlen(name) + 1);
            strcpy(key, name);
            val = malloc(strlen(value) + 1);
            strcpy(val, value);
            HASH_INSERT(ini->hashtable, new_key, val); 
        }
    }

    fclose(fp);
    return INI_OK;
}

int ini_load_config_file(struct INI_Context_ * ini, const char *filename) {
    FILE *fp;
    if((fp = fopen(filename, "rt")) == NULL) {
        return INIERR_OPEN;
    }

    return ini_load_config_fp(ini, fp);
}

const char *ini_get_value(
    const struct INI_Context_ * ini,
    const char *section,
    const char *var) {

    INI_Hash_Key key, *pkey;
    HASH_ELEMENT(INI_Hash_Key* , const char*) result;

    key.section_name = section;
    key.key_name = var;
    pkey = &key;

    if(!HASH_FIND(ini->hashtable, pkey, key_comp, result)) {
        return NULL;
    } else {
        return result.value;
    }
}

void ini_split_var_value(
    const char *string,
    char *var, int var_len,
    char *val, int val_len) {

    // Grab the variable name
    while(*string != '=' &&
          *string != ' ' &&
          *string != '\t' &&
          *string != 0) {

        *var++ = *string++;
    }

    // Null-terminate
    *var = *val = 0;

    // Check to see if we are at the end of the string
    if(*string == 0) return;

    // Find the beginning of the value
    while(*string == '=' ||
          *string == ' ' ||
          *string == '\t') {
        string++;
    }

    // Check to see if we are at the end of the string
    if(*string == 0) return;

    // Set the value
    while(*string != '\r' &&
          *string != '\n' &&
          *string != '\0') {
        *val++ = *string++;
    }

    // Null-terminate
    *val = 0;
}

