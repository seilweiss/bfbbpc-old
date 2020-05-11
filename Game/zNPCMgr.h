#ifndef ZNPCMGR_H
#define ZNPCMGR_H

#include "xEnt.h"

void zNPCMgr_Startup();
void zNPCMgr_Shutdown();
void zNPCMgr_scenePrepare(int npccnt);
xEnt *zNPCMgr_createNPCInst(xEntAsset *assdat);

#endif