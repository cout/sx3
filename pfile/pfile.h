#ifndef _SX3_PFILE_H_
#define _SX3_PFILE_H_
#define SX3_PFILE_PATH_LENGTH 56
#include <stdio.h>

struct PFile {
    FILE*   file;
    long    offset;
    long    length;
    struct  PSubFile* toc;
    long    numSubFiles;
};

struct PSubFile {
    FILE*   file;
    char*   path;
    long    offset;
    long    length;
    long    position;
    int     open;
};

//--------------------------------------------------------------------------
struct PFile* pakOpen(const char* name);
int pakClose(struct PFile* file);
struct PSubFile* pakOpenSub(struct PFile* file, const char* name);
int pakCloseSub(struct PSubFile* file);
int pakRead(void* buf, size_t size, size_t count, struct PSubFile* file);
int pakSeek(long offset, int origin, struct PSubFile* file);
long pakTell(struct PSubFile* file);
char* pakFind(const char* name, struct PFile* file, int skip);
int pakScanf(struct PSubFile* file, const char* format, ...);

#endif /* _SX3_PFILE_H_ */
