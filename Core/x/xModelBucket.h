#ifndef XMODELBUCKET_H
#define XMODELBUCKET_H

#include <rwcore.h>
#include <rpworld.h>

struct xModelInstance;

struct xModelBucket
{
	RpAtomic *Data;
	RpAtomic *OriginalData;
	xModelInstance *List;
	int ClipFlags;
	unsigned int PipeFlags;
};

#endif