#include <stdio.h>
#include <stdlib.h>
#include "PakFile.h"

// close a Pfile. Returns errors if there are open subfiles or 
// fail to close the underlieing file.
int pakClose(struct PFile* file) {
    int i;    // for loop counter
    struct PSubFile* toc = file->toc; // pointer to file's toc

    // iterate through the subfiles looking for open files.
    // if one is found, return error
    for(i = file->numSubFiles - 1; i > 0; i--) {
        if(0 == toc[i].open) {
            return -1;
        }// end if
    }// end for

    // try and close the accual file, error out if fail
    if(0 != fclose(file->file)) {
        return -1;
    }// end if

    // free the path for each subfile listing
    for(i = file->numSubFiles - 1; i > 0; i--) {
        free(toc[i].path);
    }// end for
    free(file->toc);// free the toc first, then the file itself
    free(file);
    return 0;
}// end pClose
