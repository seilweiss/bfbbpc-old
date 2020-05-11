#ifndef ZENTSIMPLEOBJ_H
#define ZENTSIMPLEOBJ_H

#include "zEnt.h"

struct xSimpleObjAsset
{
	float animSpeed;
	unsigned int initAnimState;
	unsigned char collType;
	unsigned char flags;
};

struct zEntSimpleObj : zEnt
{
	xSimpleObjAsset *sasset;
	unsigned int sflags;
	void *anim;
	float animTime;
};

void zEntSimpleObj_Init(void *ent, void *asset);
void zEntTrackPhysics_Init(void *ent, void *asset);

#endif