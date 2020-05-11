#include "zMenu.h"

#include "iTime.h"
#include "xMath.h"
#include "zScene.h"
#include "xCamera.h"
#include "zMain.h"
#include "iSystem.h"
#include "zCamera.h"
#include "zMusic.h"

#include "print.h"

static int sFirstBoot = 1;

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
    BFBBSTUB("zMenuSetup");
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