#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "PakFile.h"

// Open the pak file names 'name'. 
struct PFile* pakOpen(const char* name) {
	int i, subs;
	struct PFile* r = (struct PFile*) malloc(sizeof(struct PFile));
	struct PakHeader h;
	struct PakTOC* toc;
	struct PSubFile* currentSub;

	// if malloc failed to assign memory to r
	if(NULL == r) {
		return NULL;
	}// end if
//17
	r->file = fopen(name, "rb");

	// if test, in sequence, if fopen failed to open name, fread fails
	// read the header info, if the header is of a pak file, if the toc
	// is corrupt, and if fseek fails
	if(
	   (NULL == r->file) ||
	   (1 != fread(&h, sizeof(struct PakHeader), 1, r->file)) ||
	   (0 != strncmp("PACK", h.type, 4)) ||
	   (0 != (h.length % sizeof(struct PakTOC))) ||
	   (0 != fseek(r->file, h.offset, SEEK_SET))
	   ) {
/*		printf("%s\n%c%c%c%c\n%u\n%u\n%i\n%i\n", "debug info: ",
			h.type[0], h.type[1], h.type[2], h.type[3],
			h.offset, h.length, sizeof(struct PakTOC),
			sizeof(struct PakHeader));
*/		return NULL;
	}// end if
//36
	r->offset = h.offset;
	r->length = h.length;
	r->numSubFiles = subs = h.length / sizeof(struct PakTOC);
	r->toc = (struct PSubFile*) malloc(sizeof(struct PSubFile) * subs);
	toc = (struct PakTOC*) malloc(sizeof(struct PakTOC) * subs);

	// if test r->toc to insure malloc didn't fail and if fread failed
	if(
	   (NULL == r->toc) ||
	   (NULL == toc) ||
	   (fread(toc, sizeof(struct PakTOC), subs, r->file) != subs)
	   ) {
		printf("%s\n", "failed NULL test 3 or seek failure");
		return NULL;
	}// end if
//52
	currentSub = (r->toc);

	// for loop copies contents of toc to t->toc and inisalizes other
	// varables in r
	for(i = 0; i < subs; i++, (currentSub)++) {
		createPSubFile(currentSub, &toc[i], r->file);
	}// end for

	free(toc);
	return r;
}// end pOpen
