#include <stdio.h>
#include "c_hash.h"
#include "c_algo.h"

#define MAX_ELEMENTS 1000

size_t string_hash(char **x) {
    if(*x == 0) {
        return **x;
    } else {
        return **x * *(*x + 1);
    }
}

CTL_BOOL string_comp(char **x, char **y) {
    return strcmp(*x, *y);
}

char * random_string(size_t size) {
    char * foo = malloc(size + 1);
    foo[size] = '\0';
    for(; size > 0; --size) {
        foo[size - 1] = 'A' + rand() % 26;
    }
    return foo;
}

int main() {
    HASH(char *, char *) h;
    int j;
    char **keys;
    char **values;
    HASH_ELEMENT(char *, char *) result;

    HASH_INIT(h, 17, string_hash);
    keys = malloc(MAX_ELEMENTS * sizeof(char *));
    values = malloc(MAX_ELEMENTS * sizeof(char *));

    for(j = 0; j < MAX_ELEMENTS; ++j) {
        keys[j] = random_string(10);
        values[j] = random_string(10);
        HASH_INSERT(h, keys[j], values[j]);
        // printf("%d: (%s, %s) -- %d\n",
        //     j, keys[j], values[j], string_hash(&keys[j]) % 17);
    }

    for(j = 0; j < MAX_ELEMENTS; ++j) {
        if(!HASH_FIND(h, keys[j], string_comp, result)) {
            printf("Uh oh!  Didn't find element %d -- %s\n", j, keys[j]);
        } else {
            if(strcmp(result.key, keys[j])) {
                printf("Found key %d, but it was wrong (%s)\n", j, result.key);
            }
            if(strcmp(result.value, values[j])) {
                printf("Found value %d, but it was wrong (%s)\n", j, result.value);
            }
        }
    }

    HASH_FREE(h);
    for(j = 0; j < MAX_ELEMENTS; ++j) {
        free(keys[j]);
        free(values[j]);
    }
    free(keys);
    free(values);

    return 0;
}
