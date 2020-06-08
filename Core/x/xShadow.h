#ifndef XSHADOW_H
#define XSHADOW_H

#include <rwcore.h>
#include <rpworld.h>

void xShadowInit();
void xShadowSetWorld(RpWorld *world);
void xShadowManager_Init(int numEnts);
void xShadowManager_Render();

#endif