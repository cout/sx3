#include <stdlib.h>
#include <string.h>
#include "PakFile.h"

//Fuction creates a PSubFile list at 'destination' using 'source' and 'file'
void createPSubFile(struct PSubFile* destination, struct PakTOC* source,
    FILE* file) {
    int pathLength = strlen(source->path);

    // check to see if strlen returned a length > SX3_PFILE_PATH_LENGTH,
    // indecating that all of the space for the path was used, there was
    // no terminating char. If so, will create a space large enough for the
    // string and a terminater. Otherwise just create space size pathLenght
    if(SX3_PFILE_PATH_LENGTH > pathLength) {
        destination->path = (char*) malloc(pathLength);
        strcpy(destination->path, source->path);
    } else {
        destination->path = (char*) malloc(SX3_PFILE_PATH_LENGTH + 1);
        strncpy(destination->path, source->path, SX3_PFILE_PATH_LENGTH);
        destination->path[SX3_PFILE_PATH_LENGTH] = '\n';
    }// end if... else
//95
    destination->file = file;
    destination->offset = source->offset;
    destination->length = source->length;
    destination->position = 0;
    destination->open = 1;
}// end createTOCRecord
