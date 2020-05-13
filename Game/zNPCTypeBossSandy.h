#ifndef ZNPCTYPEBOSSSANDY_H
#define ZNPCTYPEBOSSSANDY_H

struct xAnimTable;
struct zScene;

xAnimTable *ZNPC_AnimTable_BossSandy();
xAnimTable *ZNPC_AnimTable_BossSandyHead();
void zNPCBSandy_AddBoundEntsToGrid(zScene *zsc);

#endif