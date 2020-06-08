#ifndef XFX_H
#define XFX_H

#include <rwcore.h>
#include <rpworld.h>

void xFXInit();
void xFXRingRender();
void xFX_SceneEnter(RpWorld *world);
void xFXPreAllocMatFX(RpClump *clump);
void xFXFireworksInit(const char *fireworksTrailEmitter, const char *fireworksEmitter1,
                      const char *fireworksEmitter2, const char *fireworksSound,
                      const char *fireworksLaunchSound);
void xFXStreakInit();
void xFXStreakRender();
void xFXShineInit();
void xFXShineRender();
void xFXRibbonSceneEnter();
void xFXRibbonRender();
void xFXAuraRender();
void xFXStartup();
void xFXShutdown();
void xFXSceneInit();
void xFXSceneSetup();
void xFXScenePrepare();
void xFXSceneFinish();

#endif