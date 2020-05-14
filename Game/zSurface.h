#ifndef ZSURFACE_H
#define ZSURFACE_H

#include "xSurface.h"

void zSurfaceInit();
void zSurfaceRegisterMapper(unsigned int assetId);
void zSurfaceExit();
void zSurfaceSetup(xSurface *s);

#endif