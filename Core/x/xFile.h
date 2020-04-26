#ifndef XFILE_H
#define XFILE_H

#include "iFile.h"

struct tag_xFile
{
	char relname[32];
	tag_iFile ps;
	void *user_data;
};

#endif