#ifndef IFILE_H
#define IFILE_H

#include <windows.h>

#define IFILE_ISOPEN 0x1

#define IFILE_SEEK_SET 0
#define IFILE_SEEK_CUR 1
#define IFILE_SEEK_END 2

struct tag_iFile
{
    unsigned int flags;
    char path[256];
    HANDLE handle;
    OVERLAPPED overlapped;
    int asynckey;
    int offset;
    int length;
};

enum IFILE_READSECTOR_STATUS
{
    IFILE_RDSTAT_NOOP,
    IFILE_RDSTAT_INPROG,
    IFILE_RDSTAT_DONE,
    IFILE_RDSTAT_FAIL,
    IFILE_RDSTAT_QUEUED,
    IFILE_RDSTAT_EXPIRED
};

struct tag_xFile;

typedef void(*iFileReadCallBack)(tag_xFile *file);

void iFileInit();
void iFileFullPath(const char *relname, char *fullname);
unsigned int iFileOpen(const char *name, int flags, tag_xFile *file);
unsigned int iFileGetSize(tag_xFile *file);
unsigned int iFileRead(tag_xFile *file, void *buf, unsigned int size);
unsigned int iFileClose(tag_xFile *file);
int iFileSeek(tag_xFile *file, int offset, int whence);
int iFileReadAsync(tag_xFile *file, void *buf, unsigned int aSize,
                   iFileReadCallBack callback, int priority);
IFILE_READSECTOR_STATUS iFileReadAsyncStatus(int key, int *amtToFar);
void iFileAsyncService();
void iFileSetPath(char *path);
unsigned int iFileFind(const char *name, tag_xFile *file);
void iFileGetInfo(tag_xFile *file, unsigned int *offset, unsigned int *length);

#endif