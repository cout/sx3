#include <stdio.h>
#include "PakFile.h"

// sets the position pointer to offset
int pakSeek(long offset, int origin, struct PSubFile* file) {
	// determains from where to set the position from, the
	// beginning, the current postion, or the end
	if(0 != file->open) {
		return -1;
	}// end if
	switch(origin) {
		case SEEK_SET:// start from the beginning
			if(file->length < offset) {
				file->position = offset;
			} else {return -1;}// end if... else
			break;
		case SEEK_CUR:// start from the current position
			if(file->length < (offset + file->position)) {
				file->position += offset;
			} else {return -1;}// end if... else
			break;
		case SEEK_END:// start from the end
			if(file->length < offset) {
				file->position = file->length - offset;
			} else {return -1;}// end if... else
			break;
		default:
			return -1;
	}// end switch
	return 0;
}// end pSeek
