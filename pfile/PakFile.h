#ifndef _SX3_PAKFILE_H_
#define _SX3_PAKFILE_H_

#include <stdio.h>
#include "pfile.h"

#define SX3_PFILE_PATH_LENGTH 56

struct PakHeader {
	char 			type[4];
	unsigned long	offset;
	unsigned long	length;
};

struct PakTOC {
	char			path[SX3_PFILE_PATH_LENGTH];
	unsigned long	offset;
	unsigned long	length;
};

void createPSubFile(struct PSubFile* destination, struct PakTOC* source,
        FILE* file);

#endif /* !_SX3_PAKFILE_H_ */
