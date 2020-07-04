#ifndef ZBUSSTOP_H
#define ZBUSSTOP_H

#include "zEnt.h"
#include "xDynAsset.h"

struct busstop_asset : xDynAsset
{
	unsigned int marker;
	unsigned int character;
	unsigned int cameraID;
	unsigned int busID;
	float delay;
};

struct zBusStop : xBase
{
	busstop_asset *basset;
	zEnt *bus;
	xVec3 pos;
	unsigned int currState;
	unsigned int prevState;
	float switchTimer;
};

struct xScene;

void zBusStop_Setup(zBusStop *bstop);
void zBusStop_Update(xBase *to, xScene *sc, float dt);

#endif