#include "PakFile.h"

int pakCloseSub(struct PSubFile* file);

// Closes the subfile 'file'
int pakCloseSub(struct PSubFile* file) {
	if(0 == file->open) {
		file->open = 1;
		return 0;
	} else {
		return -1;
	}// end if... else
}// end pCloseSub
