#include <stdio.h>
#include "PakFile.h"

// Reads count number of objects, each of size bytes and writes them to
// buf. if that would go passed the end of the file, only enough to
// reach the end are read.
int pakRead(void* buf, size_t size, size_t count, struct PSubFile* file) {
	int read;// the number of objects accually read

	// determain if the file is open then try fseek. If fail, error out
	if(
	   (0 != file->open) ||
	   (0 != fseek(file->file, file->offset + file->position, SEEK_SET))
	   ) {
		return -1;
	}// end if

	// Check to see if read will pass end of the subfile. If so, read
	// only what is in the subfile; stop before the end
	if(file->position + (size * count) > file->length) {
		read = fread(buf, size, (file->length - file->position) / size, 
				 file->file);
	} else {
		read = fread(buf, size, count, file->file);
	} // end if... else

	file->position += read * size;//update position, very important
	return read;
}// end pRead
