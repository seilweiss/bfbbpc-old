#ifndef ZENT_H
#define ZENT_H

#include "xEnt.h"
#include "xAnim.h"

struct zEnt : xEnt
{
    xAnimTable *atbl;
};

struct zScene;

void zEntUpdate(zEnt *ent, zScene *scene, float elapsedSec);
void zEntParseModelInfo(xEnt *ent, unsigned int assetID);
void zEntInit(zEnt *ent, xEntAsset *asset, unsigned int type);
void zEntEventAll(xBase *from, unsigned int fromEvent, unsigned int toEvent,
                  float *toParam);
void zEntEventAllOfType(unsigned int toEvent, unsigned int type);
xModelInstance *zEntRecurseModelInfo(void *info, xEnt *ent);
xModelAssetParam *zEntGetModelParams(unsigned int assetID, unsigned int *size);

#endif