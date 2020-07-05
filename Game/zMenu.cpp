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
#include "zGameState.h"
#include "zGameExtras.h"
#include "xScrFx.h"
#include "zFMV.h"
#include "xString.h"
#include "xDraw.h"
#include "iTRC.h"
#include "xDebug.h"
#include "zSaveLoad.h"
#include "xTRC.h"
#include "iWindow.h"

#include "DebugText.h"

#include "print.h"

#define ATTRACT_MODE_TIME 48.264f

bool menu_fmv_played = false;

static int sFirstBoot = 1;
static float time_elapsed = 0.01f;
static float time_last = 0.0f;
static float time_current = 0.0f;
static float sAttractMode_timer = 0.0f;
static int sInMenu = 0;
static float holdTmr = 10.0f;

static int zMenuIsPaused();
static unsigned int zMenuUpdateMode();
static int zMenuLoopContinue();

int zMenuRunning()
{
    return sInMenu;
}

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

    sAttractMode_timer = ATTRACT_MODE_TIME;

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
    unsigned int s = 0;

    if (sFirstBoot)
    {
        zGameModeSwitch(eGameMode_Intro);
        zGameStateSwitch(eIntroState_Sony);
        zMusicNotify(eMusicSituation_Unk00);
    }
    else
    {
        zGameModeSwitch(eGameMode_Title);
        zGameStateSwitch(eTitleState_Start);
        zMusicNotify(eMusicSituation_Unk00);
    }

    time_last = iTimeTicksToSecs(1) * iTimeGet() - ONE_FRAME;

    zGameExtras_NewGameReset();

    iColor_tag black = { 0, 0, 0, 255 };
    iColor_tag clear = { 0, 0, 0, 0 };

    xScrFxFade(&black, &clear, 0.0f, NULL, 1);

    int ostrich_delay = 10;

    time_last = iTimeTicksToSecs(1) * iTimeGet() - ONE_FRAME;

    do
    {
        int draw_black = 0;

        if (zGameModeGet() == eGameMode_Intro)
        {
            switch (zGameStateGet())
            {
            case eIntroState_Sony:
            {
                draw_black = 1;

                zGameStateSwitch(eIntroState_License);
                xScrFxFade(&clear, &black, 0.0f, NULL, 1);

                break;
            }

            case eIntroState_License:
            {
                draw_black = 1;

                zFMVPlay(zFMVFileGetName(eFMVFile_LogoNick), XPAD_BUTTON_X, 2.0f, true, false);
                zFMVPlay(zFMVFileGetName(eFMVFile_LogoTHQ), XPAD_BUTTON_X, 2.0f, true, false);
                zFMVPlay(zFMVFileGetName(eFMVFile_LogoRW), XPAD_BUTTON_X, 2.0f, true, false);

                holdTmr = 10.0f;

                zGameModeSwitch(eGameMode_Title);
                zGameStateSwitch(eTitleState_Attract);

                xScrFxFade(&black, &clear, 0.1f, NULL, 1);

                time_last = iTimeTicksToSecs(1) * iTimeGet() - ONE_FRAME;

                break;
            }
            }
        }

        time_current = iTimeTicksToSecs(1) * iTimeGet();
        time_elapsed = time_current - time_last;

        if (menu_fmv_played && time_elapsed > ONE_FRAME)
        {
            time_last = time_current - ONE_FRAME;
            time_elapsed = ONE_FRAME;
        }

        menu_fmv_played = false;

        if (time_elapsed < 0.0f)
        {
            time_elapsed = ONE_FRAME;
        }

        if (zGameModeGet() == eGameMode_Title)
        {
            sAttractMode_timer -= time_elapsed;

            if (sAttractMode_timer < 0.0f)
            {
                xEnt *title1 = (xEnt *)zSceneFindObject(xStrHash("MNU3 PRESS START 02 UIF"));
                xEnt *title2 = (xEnt *)zSceneFindObject(xStrHash("MNU3 START NEWGAME UIF"));

                if ((title1 && xEntIsVisible(title1)) ||
                    (title2 && xEntIsVisible(title2)))
                {
                    zGameStateSwitch(eTitleState_Attract);
                }
                else
                {
                    sAttractMode_timer = ATTRACT_MODE_TIME;
                }
            }

            if (zGameStateGet() == eTitleState_Attract)
            {
                if (sFirstBoot)
                {
                    zMenuFirstBootSet(0);
                }
                else
                {
                    zMenuFMVPlay(zFMVFileGetName(eFMVFile_Demo1), XPAD_BUTTON_ALL, 0.1f,
                                 true, globals.firstStartPressed);
                }

                zGameStateSwitch(eTitleState_Start);

                sAttractMode_timer = ATTRACT_MODE_TIME;
            }
            else
            {
                zMusicUpdate(time_elapsed);
            }

            if (mPad[globals.currentActivePad].pressed != 0)
            {
                sAttractMode_timer = ATTRACT_MODE_TIME;
            }
        }

        time_last = time_current;

        xPadUpdate(globals.currentActivePad, time_elapsed);

        if (!globals.firstStartPressed)
        {
            xPadUpdate(1, time_elapsed);
            xPadUpdate(2, time_elapsed);
            xPadUpdate(3, time_elapsed);
        }

        xDrawBegin();

        int paused = zMenuIsPaused();

        zSceneUpdate(time_elapsed);

        if (!paused)
        {
            globals.player.ent.update(&globals.player.ent, globals.sceneCur, time_elapsed);
        }

        xSndUpdate();
        iTRCDisk::CheckDVDAndResetState();

        zCameraUpdate(&globals.camera, time_elapsed);

        xCameraBegin(&globals.camera, 1);

        zSceneRender();

        xDebugUpdate();

        if (draw_black)
        {
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);
            RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDZERO);
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDZERO);

            xScrFxDrawScreenSizeRectangle();
        }

        DebugText_Render();

        xDrawEnd();

        xCameraEnd(&globals.camera, time_elapsed, 1);
        xCameraShowRaster(&globals.camera);

        s = zMenuUpdateMode();

        if (globals.sceneCur->pendingPortal)
        {
            char *src = (char *)&globals.sceneCur->pendingPortal->passet->sceneID;
            char *tgt = (char *)&s;

            tgt[0] = src[3];
            tgt[1] = src[2];
            tgt[2] = src[1];
            tgt[3] = src[0];

            if (tgt[3] < '0' || tgt[3] > '9')
            {
                memcpy(tgt, src, 4);
            }

            zGameModeSwitch(eGameMode_Game);
            zGameStateSwitch(eGameState_FirstTime);
        }

        gFrameCount++;

        if (ostrich_delay > 0)
        {
            ostrich_delay--;
        }
        else
        {
            zGameSetOstrich(eGameOstrich_InScene);

            if (gTrcPad[0].state != TRC_PadInserted)
            {
                globals.dontShowPadMessageDuringLoadingOrCutScene = true;
                xTRCPad(gTrcPad[0].id, TRC_PadMissing);
            }
        }

        iWindowUpdate();
        iCameraUpdateSize(globals.camera.lo_cam, SCREEN_WIDTH, SCREEN_HEIGHT,
                          globals.camera.fov);
    }
    while (zMenuLoopContinue());

    zGameExtras_NewGameReset();

    return s;
}

static int zMenuIsPaused()
{
    return 0;
}

static int zMenuLoopContinue()
{
    return (gGameMode != eGameMode_Game);
}

unsigned int zMenuUpdateMode()
{
    unsigned int retVal = 0;

    if (gGameMode == eGameMode_Load)
    {
        float elapsed1 = iTimeGet() - time_last;

        retVal = zSaveLoad_LoadLoop();

        if (retVal == '0000')
        {
            retVal = 0;
        }
        else if (retVal == 'MNU3')
        {
            retVal = 'HB00';
        }

        time_last = elapsed1 + iTimeGet();
    }
    else if (gGameMode == eGameMode_Save)
    {
        zSaveLoad_LoadLoop();

        globals.autoSaveFeature = true;

        retVal = 'HB00';
    }

    return retVal;
}

void zMenuFMVPlay(char *filename, unsigned int buttons, float time, bool skippable,
                  bool lockController)
{
    if (filename)
    {
        zFMVPlay(filename, buttons, time, skippable, lockController);

        time_last = (float)iTimeGet();
        time_current = time_last;

        sAttractMode_timer = ATTRACT_MODE_TIME;
    }
}

int zMenuIsFirstBoot()
{
    return sFirstBoot;
}

void zMenuFirstBootSet(int firstBoot)
{
    sFirstBoot = firstBoot;
}