#ifndef ZNPCMGR_H
#define ZNPCMGR_H

#include "xEnt.h"

void zNPCMgr_Startup();
void zNPCMgr_Shutdown();
void zNPCMgr_scenePrepare(int npccnt);
void zNPCMgr_sceneFinish();
void zNPCMgr_scenePostInit();
void zNPCMgr_scenePostSetup();
void zNPCMgr_sceneTimestep(xScene *xscn, float dt);
void zNPCMgr_scenePostRender();
void zNPCMgr_scenePostParticleRender();
xEnt *zNPCMgr_createNPCInst(xEntAsset *assdat);

#endif