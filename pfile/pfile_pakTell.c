#include "PakFile.h"

// returns the current position in 'file'
long pakTell(struct PSubFile* file) {
	if(0 != file->open) {
		return -1;
	} else {
		return file->position;
	}// end if... else
}// end pTell
