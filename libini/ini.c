#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctl/c_hash.h>

typedef struct INI_HASH_KEY_{
    char *section_name;
    char *key_name;
} INI_Hash_Key;
 
typedef struct {
    HASH(INI_Hash_Key*, char*) hashtable; 
} INI_Context;

#define LIBINI
#include "ini.h"

size_t ini_generate_hash_val(INI_Hash_Key **key);

INI_Context* ini_new_context(void) {
    INI_Context *new_context = malloc(sizeof(INI_Context));
    // We're using 29 cause it seemed like a good number, oh, it's prime too :-) 
    HASH_INIT(new_context->hashtable, 29, ini_generate_hash_val);
    return new_context;
}

void ini_free_context(INI_Context *context) {
    //We are assuming the the program is ending, so we're not going to the
    //trouble of freeing the keys and stuff. Hopefully ctl will handle this soon.
    HASH_FREE(context->hashtable);
    free(context);
}

size_t ini_generate_hash_val(INI_Hash_Key **key) {
    //Generating the hash val by multipling the ascii values of the strings.
    size_t key_val = 0;
    char *ptr = (*key)->section_name;
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

CTL_BOOL key_comp(INI_Hash_Key **a, INI_Hash_Key **b) {
    return (strcmp((*a)->section_name,(*b)->section_name) && 
            strcmp((*a)->key_name,(*b)->key_name));
}

int ini_load_config_fp(INI_Context *ini, FILE *fp) {
    //These function reads in the contents of a ini file, line by line. 
    char s[1024];
    char *section_name = 0; //stores the current section name, we only see it
                            //on the line that starts the ini section. 

    while(TRUE) {
        fgets(s, sizeof(s), fp);
        if(feof(fp)) break;

        if(s[0] == '#' || s[0] == ';') {

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
            char* val;
            INI_Hash_Key *new_key = 0;
            ini_split_var_value(s, name, sizeof(name), value, sizeof(value));

            //Should the below event be recorded anywhere? (TM, SB)
            if(!section_name) continue;

            new_key = malloc(sizeof(INI_Hash_Key));
            new_key->section_name = section_name;
            new_key->key_name = malloc(strlen(name) + 1);
            strcpy(new_key->key_name, name);
            val = malloc(strlen(value) + 1);
            strcpy(val, value);
            HASH_INSERT(ini->hashtable, new_key, val); 
        }
    }

    fclose(fp);
    return INI_OK;
}

int ini_load_config_file(INI_Context * ini, const char *filename) {
    FILE *fp;
    if((fp = fopen(filename, "rt")) == NULL) {
        return INIERR_OPEN;
    }

    return ini_load_config_fp(ini, fp);
}

const char *ini_get_value(
    INI_Context * ini,
    const char *section,
    const char *var) {
    //This function is a glorified hash look up; we just put section and 
    //var in a new key and get the value.
    INI_Hash_Key* key = malloc(sizeof(INI_Hash_Key));
    HASH_ELEMENT(INI_Hash_Key* , char*) result;
    //We do these string copies cause we would otherwise be passing const 
    //parameters to a function in which they would not be const.
    key->section_name = malloc(strlen(section) + 1);
    strcpy(key->section_name, section);
    key->key_name = malloc(strlen(var) + 1);
    strcpy(key->key_name, var);

    if(!HASH_FIND(ini->hashtable, key, key_comp, result)) {
        free(key);
        return NULL;
    } else {
        free(key);
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

