#include <stdio.h>
#include <string.h>
#include "PakFile.h"

// opens a sub file. Returns NULL if name is not found or it is already open
struct PSubFile* pakOpenSub(struct PFile* file, const char* name) {
	int i;	// for loop counter
	struct PSubFile* current = file->toc; // pointer to current subfile listing

	// iterate through the subfiles looking for name.
	// Will only return a subfile if it is not already open
	for(i = file->numSubFiles; i > 0; i--, current++) {
		if(
		   (0 == strncmp(current->path, name, SX3_PFILE_PATH_LENGTH)) &&
		   (0 != current->open)
		   ) {
			current->open = 0;
			return current;
		}// end if
	}// end for
	return NULL;
}// end pOpenSub
