#ifndef IMODEL_H
#define IMODEL_H

#include <rwcore.h>
#include <rpworld.h>

void iModelInit();
int iModelCull(RpAtomic *model, RwMatrix *mat);

#endif