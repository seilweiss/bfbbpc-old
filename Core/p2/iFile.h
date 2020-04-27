#ifndef IFILE_H
#define IFILE_H

struct tag_iFile
{
	unsigned int flags;
	char path[128];
	int fd;
	int offset;
	int length;
};

void iFileInit();

#endif