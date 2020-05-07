#ifndef XFX_H
#define XFX_H

#include <rwcore.h>
#include <rpworld.h>

void xFXInit();
void xFX_SceneEnter(RpWorld *world);
void xFXPreAllocMatFX(RpClump *clump);
void xFXStartup();
void xFXShutdown();

#endif