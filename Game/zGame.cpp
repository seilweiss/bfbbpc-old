#include "zGame.h"

#include "zGameState.h"
#include "zMain.h"
#include "iCamera.h"
#include "iSystem.h"
#include "zMenu.h"
#include "xDraw.h"
#include "iTRC.h"
#include "xstransvc.h"
#include "xDebug.h"
#include "zFX.h"

#include "print.h"

#include <rwcore.h>
#include <rpworld.h>

#include <string.h>

unsigned int gGameSfxReport;
float sTimeElapsed;
unsigned int gSoak;

unsigned int loadMeter;

eGameWhereAmI gGameWhereAmI;

unsigned char sHackSmoothedUpdate;

RwCamera *sGameScreenTransCam;

static RpWorld *World;
static RpLight *DirectionalLight;

unsigned int bgID = 0x1D33B0BB; // fx_boomball_smoke.RW3
float bgu1 = 0.0f;
float bgv1 = 0.0f;
float bgu2 = 1.333f;
float bgv2 = 1.0f;
unsigned char bgr = 96;
unsigned char bgg = 96;
unsigned char bgb = 96;
unsigned char bga = 128;

void zGameInit(unsigned int theSceneID)
{
    BFBBSTUB("zGameInit");
}

void zGameExit()
{
    BFBBSTUB("zGameExit");
}

void zGameSetup()
{
    BFBBSTUB("zGameSetup");
}

void zGameLoop()
{
    BFBBSTUB("zGameLoop");
}

void zGameTakeSnapShot(RwCamera *cam)
{
    return;
}

void zGameUpdateTransitionBubbles()
{
    BFBBSTUB("zGameUpdateTransitionBubbles");
}

void zGameScreenTransitionBegin()
{
    RwFrame *frame;
    RwRGBAReal col;
    RwBBox box;

    gGameWhereAmI = eGameWhere_TransitionBegin;
    zGameSetOstrich(eGameOstrich_Loading);

    globals.dontShowPadMessageDuringLoadingOrCutScene = false;

    sGameScreenTransCam = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (sGameScreenTransCam)
    {
        DirectionalLight = RpLightCreate(rpLIGHTDIRECTIONAL);

        if (DirectionalLight)
        {
            col.red = col.green = col.blue = 1.0f;
            col.alpha = 0.0f;

            RpLightSetColor(DirectionalLight, &col);

            frame = RwFrameCreate();
            RpLightSetFrame(DirectionalLight, frame);

            box.sup.x = 10000.0f;
            box.sup.y = 10000.0f;
            box.sup.z = 10000.0f;
            box.inf.x = -10000.0f;
            box.inf.y = -10000.0f;
            box.inf.z = -10000.0f;

            World = RpWorldCreate(&box);

            RpWorldAddCamera(World, sGameScreenTransCam);

            gGameWhereAmI = eGameWhere_TransitionSnapShot;
            zGameTakeSnapShot(sGameScreenTransCam);
        }
    }
}

void zGameScreenTransitionUpdate(float percentComplete, const char *msg)
{
    if (!zMenuIsFirstBoot())
    {
        zGameScreenTransitionUpdate(percentComplete, msg, NULL);
    }
}

void zGameScreenTransitionUpdate(float percentComplete, const char *msg,
                                 unsigned char *rgba)
{
    RwRGBA back_col;
    RwTexture *tex;
    RwRaster *ras;
    float recipCamZ;
    RwIm2DVertex vx[4];
    float z;

    gGameWhereAmI = eGameWhere_TransitionUpdate;

    if (!zMenuIsFirstBoot())
    {
        back_col = { 0, 0, 0, 255 };

        if (rgba)
        {
            back_col.red = rgba[0];
            back_col.green = rgba[1];
            back_col.blue = rgba[2];
            back_col.alpha = rgba[3];
        }

        gGameWhereAmI = eGameWhere_TransitionPadUpdate;

        xPadUpdate(globals.currentActivePad, sTimeElapsed);
        xDrawBegin();

        if (sGameScreenTransCam)
        {
            gGameWhereAmI = eGameWhere_TransitionTRCCheck;

            iTRCDisk::CheckDVDAndResetState();

            gGameWhereAmI = eGameWhere_TransitionCameraClear;

            RwCameraClear(sGameScreenTransCam, &back_col,
                          rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

            gGameWhereAmI = eGameWhere_TransitionCameraBegin;

            RwCameraBeginUpdate(sGameScreenTransCam);

            gGameWhereAmI = eGameWhere_TransitionRenderBackground;

            tex = (RwTexture *)xSTFindAsset(bgID, NULL);

            if (tex)
            {
                ras = RwTextureGetRaster(tex);

                if (ras)
                {
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
                    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDONE);
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDZERO);
                    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
                    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);

                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)ras);

                    recipCamZ = 1.0f / RwCameraGetFarClipPlane(sGameScreenTransCam);
                    z = RwIm2DGetFarScreenZ();

                    RwIm2DVertexSetScreenX(&vx[0], 0.0f);
                    RwIm2DVertexSetScreenY(&vx[0], 0.0f);
                    RwIm2DVertexSetScreenZ(&vx[0], z);
                    RwIm2DVertexSetRecipCameraZ(&vx[0], recipCamZ);
                    RwIm2DVertexSetIntRGBA(&vx[0], bgr, bgb, bgg, bga);
                    RwIm2DVertexSetU(&vx[0], bgu1, recipCamZ);
                    RwIm2DVertexSetV(&vx[0], bgv1, recipCamZ);

                    RwIm2DVertexSetScreenX(&vx[1], 0.0f);
                    RwIm2DVertexSetScreenY(&vx[1], SCREEN_HEIGHT);
                    RwIm2DVertexSetScreenZ(&vx[1], z);
                    RwIm2DVertexSetRecipCameraZ(&vx[1], recipCamZ);
                    RwIm2DVertexSetIntRGBA(&vx[1], bgr, bgb, bgg, bga);
                    RwIm2DVertexSetU(&vx[1], bgu1, recipCamZ);
                    RwIm2DVertexSetV(&vx[1], bgv2, recipCamZ);

                    RwIm2DVertexSetScreenX(&vx[2], SCREEN_WIDTH);
                    RwIm2DVertexSetScreenY(&vx[2], 0.0f);
                    RwIm2DVertexSetScreenZ(&vx[2], z);
                    RwIm2DVertexSetRecipCameraZ(&vx[2], recipCamZ);
                    RwIm2DVertexSetIntRGBA(&vx[2], bgr, bgb, bgg, bga);
                    RwIm2DVertexSetU(&vx[2], bgu2, recipCamZ);
                    RwIm2DVertexSetV(&vx[2], bgv1, recipCamZ);

                    RwIm2DVertexSetScreenX(&vx[3], SCREEN_WIDTH);
                    RwIm2DVertexSetScreenY(&vx[3], SCREEN_HEIGHT);
                    RwIm2DVertexSetScreenZ(&vx[3], z);
                    RwIm2DVertexSetRecipCameraZ(&vx[3], recipCamZ);
                    RwIm2DVertexSetIntRGBA(&vx[3], bgr, bgb, bgg, bga);
                    RwIm2DVertexSetU(&vx[3], bgu2, recipCamZ);
                    RwIm2DVertexSetV(&vx[3], bgv2, recipCamZ);

                    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vx, 4);

                    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
                    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);
                }
            }
        }

        xprintf("Loading... %3.2f\n", percentComplete);

        if (msg)
        {
            xprintf(msg);
        }

        char meter[256] = "...";

        switch ((loadMeter / 25) - ((loadMeter / 5) * 5))
        {
        case 0:
        {
            strcpy(meter, "   ");
            break;
        }
        case 1:
        {
            strcpy(meter, ".  ");
            break;
        }
        case 2:
        {
            strcpy(meter, ".. ");
            break;
        }
        case 3:
        {
            strcpy(meter, "...");
            break;
        }
        case 4:
        {
            loadMeter = 0;
            break;
        }
        }

        loadMeter++;

        xDebugUpdate();

        gGameWhereAmI = eGameWhere_TransitionSpawnBubbles;

        zFX_SpawnBubbleWall();
        xDrawEnd();

        if (sGameScreenTransCam)
        {
            gGameWhereAmI = eGameWhere_TransitionUpdateBubbles;

            zGameUpdateTransitionBubbles();

            gGameWhereAmI = eGameWhere_TransitionCameraEnd;

            RwCameraEndUpdate(sGameScreenTransCam);

            gGameWhereAmI = eGameWhere_TransitionCameraShowRaster;

            RwCameraShowRaster(sGameScreenTransCam, NULL,
                               FULLSCREEN ? rwRASTERFLIPWAITVSYNC : 0);
        }

        gGameWhereAmI = eGameWhere_TransitionUpdateEnd;
    }
}

void zGameScreenTransitionEnd()
{
    RwFrame *frame;

    gGameWhereAmI = eGameWhere_TransitionEnd;

    _rwFrameSyncDirty();

    if (DirectionalLight)
    {
        frame = RpLightGetFrame(DirectionalLight);

        if (frame)
        {
            RwFrameDestroy(frame);
        }

        RpLightDestroy(DirectionalLight);
    }

    if (World)
    {
        if (sGameScreenTransCam)
        {
            RpWorldRemoveCamera(World, sGameScreenTransCam);
            iCameraDestroy(sGameScreenTransCam);

            sGameScreenTransCam = NULL;
        }
        
        RpWorldDestroy(World);

        World = NULL;
    }

    gGameWhereAmI = eGameWhere_TransitionEnded;
}

void zGameSetupPlayer()
{
    BFBBSTUB("zGameSetupPlayer");
}