#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "PakFile.h"

extern int vfscanf(FILE* stream, const char* format, va_list ap);
extern int vsscanf(const char* buf, const char* format, va_list ap);

int pakScanf(struct PSubFile* file, const char* format, ...) {
	va_list args;
	int r, position, length;
	char* buf;
	
	if(0 != fseek(file->file, file->offset + file->position, SEEK_SET)) {
		return 0;
	}// end if
	va_start(args, format);
	r = vfscanf(file->file, format, args);
	va_end(args);
	position = ftell(file->file);
	if ((file->offset + file->length) >= position) {
		file->position = position - file->offset;
		return r;
	} else if(0 == fseek(file->file, file->offset + file->position, SEEK_SET)) {
		length = file->length - file->position;
		buf = (char*) malloc(length);
		if(
		   (NULL == buf) &&
		   (length != fread(buf, 1, length, file->file))
		   ) { 
		    return 0; 
		}// end if
		r = vsscanf(buf, format, args);
		file->position = ftell(file->file) - file->offset;
		return r;
	} else {
		return 0;
	}// end if... else if... else
}// end pScanf
