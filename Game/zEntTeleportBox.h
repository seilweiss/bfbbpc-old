#ifndef ZENTTELEPORTBOX_H
#define ZENTTELEPORTBOX_H

#include "zEnt.h"
#include "xDynAsset.h"

struct teleport_asset : xDynAsset
{
	unsigned int marker;
	unsigned int opened;
	unsigned int launchAngle;
	unsigned int camAngle;
	unsigned int targetID;
};

struct _zEntTeleportBox : zEnt
{
	unsigned int status;
	teleport_asset *tasset;
	zEnt *target;
	unsigned int currPlrState;
	unsigned int prevPlrState;
	unsigned int jumpInAnim;
	unsigned int jumpOutAnim;
	unsigned int JOtoOpenAnim;
	unsigned int JItoOpenAnim;
	xBox trig[2];
	unsigned int currPlayerIn;
	unsigned int prevPlayerIn;
	unsigned int currPlayerNear;
	unsigned int prevPlayerNear;
	float lastdt;
	float plrCtrlTimer;
};

void zEntTeleportBox_InitAll();
void zEntTeleportBox_Setup(_zEntTeleportBox *ent);

#endif