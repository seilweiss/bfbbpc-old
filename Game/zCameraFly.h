#ifndef ZCAMERAFLY_H
#define ZCAMERAFLY_H

#include "xDynAsset.h"

struct CameraFly_asset : xDynAsset
{
	unsigned int flyID;
};

struct zCameraFly : xBase
{
	CameraFly_asset *casset;
};

void zCameraFly_Setup(zCameraFly *fly);
unsigned int zCameraFlyProcessStopEvent();

#endif