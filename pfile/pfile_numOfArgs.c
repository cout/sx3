#include "PakFile.h"

int numOfArgs(const char* format) {
    int i, r = ('%' == format[0]) ? 1 : 0;
    
    for(i = 1; '\0' != format[i]; i++) {
        if(
           ('%' == format[i]) &&
           ('%' != format[i - 1])
           ) {
            r++;
        }// end if
    }// end for
    printf("numOfArgs = %i\n", r);
    return r;
}// end numOfArgs
