#ifndef ZTAXI_H
#define ZTAXI_H

#include "zEnt.h"
#include "xDynAsset.h"

struct taxi_asset : xDynAsset
{
	unsigned int marker;
	unsigned int cameraID;
	unsigned int portalID;
	unsigned int talkBoxID;
	unsigned int textID;
	unsigned int taxiID;
	float invDelay;
	float portalDelay;
};

struct zTaxi : xBase
{
	taxi_asset *basset;
	zEnt *taxi;
	xVec3 pos;
	unsigned int currState;
	unsigned int prevState;
	float portalTimer;
	float invTimer;
	float radius;
};

void zTaxi_Setup(zTaxi *taxi);

#endif