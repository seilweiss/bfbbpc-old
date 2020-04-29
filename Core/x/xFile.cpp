#include "xFile.h"

void xFileSetUserData(tag_xFile *file, void *userdata)
{
    file->user_data = userdata;
}

XFILE_READSECTOR_STATUS xFileReadAsyncStatus(int key, int *amtToFar)
{
    return (XFILE_READSECTOR_STATUS)iFileReadAsyncStatus(key, amtToFar);
}