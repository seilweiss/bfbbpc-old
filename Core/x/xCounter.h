#ifndef XCOUNTER_H
#define XCOUNTER_H

#include "xBase.h"

struct xCounterAsset : xBaseAsset
{
	short count;
};

struct _xCounter : xBase
{
	xCounterAsset *asset;
	short count;
	unsigned char state;
	unsigned char counterFlags;
};

void xCounterInit();
void xCounterInit(void *b, void *asset);

#endif