#ifndef ZSCRIPT_H
#define ZSCRIPT_H

#include "xBase.h"

struct xScriptAsset : xBaseAsset
{
	float scriptStartTime;
	unsigned int eventCount;
};

struct _zScript : xBase
{
	xScriptAsset *tasset;
	unsigned char state;
	unsigned char more;
	unsigned char pad[2];
	float time;
};

void zScriptInit(void *b, void *tasset);

#endif