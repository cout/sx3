#include <string.h>
#include "PakFile.h"

// Finds the file 'name' in 'file'. It will skip over 'skip' number of files, 
// returning NULL if it finds less then 'skip'. Returns the complete path
// if found. 'name' can be any part of the path.
char* pakFind(const char* name, struct PFile* file, int skip) {
	int i;	// counter for the loop
	char* subStr; //beginning of the file name in the path
	struct PSubFile* current = file->toc; // pointer to the current record

	//Iterate through the toc looking for name.
	for(i = file->numSubFiles; i > 0; i--, current++) {
		subStr = strrchr(current->path, '\\');
		if(NULL != strstr(((NULL == subStr) ? current->path : subStr), name)) {
			if(0 == skip) {
				return current->path;
			} else {skip--;}// end if... else
		}// end if
	}// end for
	return NULL;
}// end pFind
