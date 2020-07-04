#ifndef IMODEL_H
#define IMODEL_H

#include <rwcore.h>
#include <rpworld.h>

void iModelInit();
RpAtomic *iModelFileNew(void *buffer, unsigned int size);
void iModelUnload(RpAtomic *userdata);
RpAtomic *iModelFile_RWMultiAtomic(RpAtomic *model);
unsigned int iModelNumBones(RpAtomic *model);
int iModelCull(RpAtomic *model, RwMatrix *mat);

#endif