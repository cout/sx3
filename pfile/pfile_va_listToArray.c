#include <stdarg.h>
#include <stdlib.h>
#include "PakFile.h"

void** va_listToArray(va_list ap, int numArgs) {
    void** r = malloc(sizeof(void*) * numArgs);
    int i;
    printf("va_listToArray:");
    for(i = 0; i < numArgs; i++) {
        r[i] = va_arg(ap, void*);
        printf("%i", i);
    }// end for
    printf("\n");
    return r;
}// end va_listToArray
