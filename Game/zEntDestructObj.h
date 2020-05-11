#ifndef ZENTDESTRUCTOBJ_H
#define ZENTDESTRUCTOBJ_H

#include "zEnt.h"
#include "zParEmitter.h"
#include "zShrapnel.h"
#include "xSFX.h"

struct zEntDestructObjAsset
{
	float animSpeed;
	unsigned int initAnimState;
	unsigned int health;
	unsigned int spawnItemID;
	unsigned int dflags;
	unsigned char collType;
	unsigned char fxType;
	unsigned char pad[2];
	float blast_radius;
	float blast_strength;
	unsigned int shrapnelID_destroy;
	unsigned int shrapnelID_hit;
	unsigned int sfx_destroy;
	unsigned int sfx_hit;
	unsigned int hitModel;
	unsigned int destroyModel;
};

struct zEntDestructObj : zEnt
{
	zEntDestructObjAsset *dasset;
	unsigned int state;
	unsigned int healthCnt;
	float fx_timer;
	zParEmitter *fx_emitter;
	float respawn_timer;
	unsigned int throw_target;
	zShrapnelAsset *shrapnel_destroy;
	zShrapnelAsset *shrapnel_hit;
	xModelInstance *base_model;
	xModelInstance *hit_model;
	xModelInstance *destroy_model;
	void(*destroy_notify)(zEntDestructObj &, void *);;
	void *notify_context;
	xSFXAsset *sfx_destroy;
	xSFXAsset *sfx_hit;
};

void zEntDestructObj_FindFX();
void zEntDestructObj_Init(void *ent, void *asset);

#endif