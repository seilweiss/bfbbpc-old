#ifndef XTIMER_H
#define XTIMER_H

#include "xBase.h"

struct xTimerAsset : xBaseAsset
{
	float seconds;
	float randomRange;
};

struct xTimer : xBase
{
	xTimerAsset *tasset;
	unsigned char state;
	unsigned char runsInPause;
	unsigned short flags;
	float secondsLeft;
};

void xTimerInit(void *b, void *tasset);

#endif