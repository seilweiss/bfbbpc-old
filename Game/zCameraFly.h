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

struct xScene;

void zCameraFly_Setup(zCameraFly *fly);
void zCameraFly_Update(xBase *to, xScene *sc, float dt);
unsigned int zCameraFlyProcessStopEvent();

#endif