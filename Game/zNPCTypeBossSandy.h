#ifndef ZNPCTYPEBOSSSANDY_H
#define ZNPCTYPEBOSSSANDY_H

struct xAnimTable;
struct zScene;

xAnimTable *ZNPC_AnimTable_BossSandy();
xAnimTable *ZNPC_AnimTable_BossSandyHead();
xAnimTable *ZNPC_AnimTable_BossSandyScoreboard();
void zNPCBSandy_AddBoundEntsToGrid(zScene *zsc);
void zNPCBSandy_GameIsPaused(zScene *zsc);

#endif