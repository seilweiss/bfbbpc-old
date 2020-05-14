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
#include "xSnd.h"

#include "print.h"

static int sFirstBoot = 1;
static float sAttractMode_timer;
static int sInMenu;

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
    zMusicKill();
    xSndStopAll(~0x4);
    xCameraExit(&globals.camera);
    zSceneExit(0);

    sInMenu = 0;
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