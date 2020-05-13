#ifndef XFX_H
#define XFX_H

#include <rwcore.h>
#include <rpworld.h>

void xFXInit();
void xFX_SceneEnter(RpWorld *world);
void xFXPreAllocMatFX(RpClump *clump);
void xFXFireworksInit(const char *fireworksTrailEmitter, const char *fireworksEmitter1,
                      const char *fireworksEmitter2, const char *fireworksSound,
                      const char *fireworksLaunchSound);
void xFXStreakInit();
void xFXShineInit();
void xFXRibbonSceneEnter();
void xFXStartup();
void xFXShutdown();
void xFXSceneInit();
void xFXSceneSetup();
void xFXScenePrepare();

#endif