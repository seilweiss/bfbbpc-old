#ifndef ZGRID_H
#define ZGRID_H

#include "xGrid.h"

extern xGrid colls_grid;
extern xGrid colls_oso_grid;

struct zScene;

void zGridInit(zScene *s);
void zGridExit(zScene *s);

#endif