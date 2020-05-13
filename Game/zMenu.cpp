#include "zMenu.h"

#include "iTime.h"
#include "xMath.h"
#include "zScene.h"
#include "xCamera.h"
#include "zMain.h"
#include "iSystem.h"
#include "zCamera.h"
#include "zMusic.h"
#include "zGame.h"
#include "xSkyDome.h"
#include "xEvent.h"

#include "print.h"

static int sFirstBoot = 1;
static float sAttractMode_timer;

void zMenuInit(unsigned int theSceneID)
{
    xsrand(iTimeGet());
    iTimeGet();
    xrand();

    zSceneInit(theSceneID, 0);

    xCameraInit(&globals.camera, SCREEN_WIDTH, SCREEN_HEIGHT);
    zCameraReset(&globals.camera);
    xCameraSetScene(&globals.camera, globals.sceneCur);

    zMusicInit();
}

void zMenuExit()
{
    BFBBSTUB("zMenuExit");
}

void zMenuSetup()
{
    globals.player.MaxHealth = 3;

    zSceneSetup();

    sAttractMode_timer = 48.264f;

    zGameSetupPlayer();
    zEnvStartingCamera(gCurEnv);
    xCameraUpdate(&globals.camera, -1.0f);
    xSkyDome_Setup();

    zEntEventAll(NULL, eEventUnknown, eEventSceneBegin, NULL);
    zEntEventAll(NULL, eEventUnknown, eEventRoomBegin, NULL);

    if (globals.updateMgr)
    {
        xUpdateCull_Update(globals.updateMgr, 100);
    }

    zEntEvent(&globals.player.ent, eEventControlOff);
}

unsigned int zMenuLoop()
{
    BFBBSTUB("zMenuLoop");
    return 0;
}

int zMenuIsFirstBoot()
{
    return sFirstBoot;
}