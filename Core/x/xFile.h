#ifndef XFILE_H
#define XFILE_H

#include "iFile.h"

struct tag_xFile
{
	char relname[32];
	tag_iFile ps;
	void *user_data;
};

enum XFILE_READSECTOR_STATUS
{
	XFILE_RDSTAT_NOOP,
	XFILE_RDSTAT_INPROG,
	XFILE_RDSTAT_DONE,
	XFILE_RDSTAT_FAIL,
	XFILE_RDSTAT_QUEUED,
	XFILE_RDSTAT_EXPIRED
};

void xFileSetUserData(tag_xFile *file, void *userdata);
XFILE_READSECTOR_STATUS xFileReadAsyncStatus(int key, int *amtToFar);

#endif