#ifndef ZSCENE_H
#define ZSCENE_H

#include "xScene.h"
#include "zPortal.h"
#include "zEnt.h"
#include "zEnv.h"
#include "xClimate.h"

struct zScene : xScene
{
    _zPortal *pendingPortal;
    union
    {
        unsigned int num_ents;
        unsigned int num_base;
    };
    union
    {
        xBase **base;
        zEnt **ents;
    };
    unsigned int num_update_base;
    xBase **update_base;
    unsigned int baseCount[72];
    xBase *baseList[72];
    _zEnv *zen;
};

struct zSceneObjectInstanceDesc;

typedef unsigned int(*zSceneObjectInstanceFunc)(zScene *s,
                                                zSceneObjectInstanceDesc *desc,
                                                unsigned int base_idx);
typedef void(*zSceneObjectInitFunc)(void *ent, void *asset);
typedef unsigned int(*zSceneObjectQuerySubObjectsFunc)(void *);

struct zSceneObjectInstanceDesc
{
    const char *name;
    int baseType;
    unsigned int assetType;
    unsigned int sizeRuntime;
    zSceneObjectInstanceFunc func;
    zSceneObjectInitFunc objectInitFunc;
    zSceneObjectQuerySubObjectsFunc querySubObjects;
};

extern unsigned char HACK_BASETYPE;
extern _tagClimate gClimate;
extern _zEnv *gCurEnv;

typedef xBase *(*zSceneForAllBaseCallBack)(xBase *b, zScene *s, void *data);

void zSceneInitEnvironmentalSoundEffect();

extern unsigned int gTransitionSceneID;

void zSceneSet(xBase *b, unsigned int idx);
xBase *zSceneFindObject(unsigned int gameID);
const char *zSceneGetName(unsigned int gameID);
const char *zSceneGetName(xBase *b);
void add_scene_tweaks();
void zSceneInit(unsigned int theSceneID, int reloadInProgress);
void zSceneForAllBase(zSceneForAllBaseCallBack func, void *data);
void zSceneSave(zScene *ent, xSerial *s);
void zSceneExit(int beginReload);
void zSceneEnableVisited(zScene *s);
int zSceneSetup_serialTraverseCB(unsigned int clientID, xSerial *xser);
void zSceneSetup();
void zSceneForAllBase(zSceneForAllBaseCallBack func, int baseType, void *data);
void zSceneMemLvlChkCB();

#endif