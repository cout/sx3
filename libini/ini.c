#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FIX ME!! This libarary uses a linked list to store its data members;
// this is very slow for lookups.
// FIX ME!! This library uses strdup, which is not portable.

typedef struct INI_ENTRY_ {
    char *name;
    char *value;
    struct INI_ENTRY_ *next_entry;
} INI_Entry;

typedef struct INI_SECTION_{
    char *name;
    INI_Entry *first_entry;
    struct INI_SECTION_ *next_section;
} INI_Section;

typedef struct {
    INI_Section *first_section;
} INI_Context;

#define LIBINI
#include "ini.h"

INI_Context* ini_new_context() {
    INI_Context *new_context = malloc(sizeof(INI_Context));
    new_context->first_section = NULL;
    return new_context;
}

void ini_free_entry(INI_Entry *entry) {
    free(entry->name);
    free(entry->value);
    free(entry);
}

void ini_free_section(INI_Section *section) {
    INI_Entry *entry = section->first_entry;
    INI_Entry *next_entry;
    while(entry) {
        next_entry = entry->next_entry;
        ini_free_entry(entry);
        entry = next_entry;
    }
    free(section->name);
    free(section);
}

void ini_free_context(INI_Context *context) {
    INI_Section *section = context->first_section;
    INI_Section *next_section;
    while(section) {
        next_section = section->next_section;
        ini_free_section(section);
        section = next_section;
    }
    free(context);
}

int ini_load_config_fp(INI_Context *ini, FILE *fp) {
    char s[1024];
    INI_Section *section = ini->first_section;
    INI_Entry *entry = 0;
    for(;;) {
        fgets(s, sizeof(s), fp);
        if(feof(fp)) break;

        if(s[0] == '#' || s[0] == ';') {

            // Comment

        } else if(s[0] == '[') {

            // New section
            char *name = s+1, *ptr = s+1;
            while(*ptr != '\r' && *ptr != '\n' && *ptr != ']' && *ptr != 0)
                ++ptr;
            *ptr = 0;

            if(!section) {
                // First section
                ini->first_section = malloc(sizeof(INI_Section));
                section = ini->first_section;
            } else {
                section->next_section = malloc(sizeof(INI_Section));
                section = section->next_section;
            }
            section->first_entry = entry = NULL;
            section->next_section = NULL;
            section->name = strdup(name);

        } else {

            // New entry
            char name[1024];
            char value[1024];
            ini_split_var_value(s, name, sizeof(name), value, sizeof(value));

            if(!section) continue;
            if(!entry) {
                // First entry
                section->first_entry = malloc(sizeof(INI_Entry));
                entry = section->first_entry;
            } else {
                entry->next_entry = malloc(sizeof(INI_Entry));
                entry = entry->next_entry;
            }
            entry->next_entry = NULL;
            entry->name = strdup(name);
            entry->value = strdup(value);

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

    INI_Section *sec = ini->first_section;
    while(sec) {
        if(!strcmp(section, sec->name)) {
            INI_Entry *ent = sec->first_entry;
            while(ent) {
                if(!strcmp(var, ent->name)) {
                    return ent->value;
                }
                ent = ent->next_entry;
            }
            return NULL;
        }
        sec = sec->next_section;
    }

    return NULL;
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
    
