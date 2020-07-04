#include "iCamera.h"

#include "iScrFX.h"
#include "zMain.h"
#include "iMath.h"
#include "xMath.h"
#include "xShadow.h"
#include "iSystem.h"

#include <rwcore.h>
#include <rpworld.h>

float sCameraNearClip = 0.05f;
float sCameraFarClip = 400.0f;

RwCamera *globalCamera;

static RwCamera *sMainGameCamera;

RwCamera *iCameraCreate(int width, int height, int mainGameCamera)
{
    RwV2d vw;
    RwCamera *camera;

    camera = RwCameraCreate();

    RwCameraSetFrame(camera, RwFrameCreate());
    RwCameraSetRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA));
    RwCameraSetZRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER));
    
    RwCameraSetFarClipPlane(camera, sCameraFarClip);
    RwCameraSetNearClipPlane(camera, sCameraNearClip);

    vw.x = 1.0f;
    vw.y = 1.0f;

    RwCameraSetViewWindow(camera, &vw);

    if (mainGameCamera)
    {
        iScrFxCameraCreated(camera);

        sMainGameCamera = camera;
    }

    return camera;
}

void iCameraDestroy(RwCamera *camera)
{
    RpWorld *pWorld;
    RwRaster *raster;
    RwFrame *frame;

    _rwFrameSyncDirty();

    pWorld = RwCameraGetWorld(camera);

    if (pWorld)
    {
        RpWorldRemoveCamera(pWorld, camera);
    }

    if (camera == sMainGameCamera)
    {
        iScrFxCameraDestroyed(camera);
        sMainGameCamera = NULL;
    }

    if (camera)
    {
        frame = RwCameraGetFrame(camera);

        if (frame)
        {
            RwCameraSetFrame(camera, NULL);
            RwFrameDestroy(frame);
        }

        raster = RwCameraGetRaster(camera);

        if (raster)
        {
            RwRasterDestroy(raster);
            RwCameraSetRaster(camera, raster);
        }

        raster = RwCameraGetZRaster(camera);

        if (raster)
        {
            RwRasterDestroy(raster);
            RwCameraSetZRaster(camera, raster);
        }

        RwCameraDestroy(camera);
    }
}

void iCameraBegin(RwCamera *cam, int clear)
{
    BFBBSTUB("iCameraBegin");

    if (clear)
    {
        /*
        if (xglobals->fog.type != rwFOGTYPENAFOGTYPE)
        {
            RwCameraClear(cam, &xglobals->fog.bgcolor,
                          rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
        }
        else
        {
            RwCameraClear(cam, NULL, rwCAMERACLEARZ);
        }
        */

        RwRGBA black = { 0, 0, 0, 0 };
        RwCameraClear(cam, &black, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
    }

    RwCameraSetNearClipPlane(cam, sCameraNearClip);
    RwCameraBeginUpdate(cam);
}

void iCameraEnd(RwCamera *cam)
{
    iScrFxCameraEndScene(cam);

    RwCameraEndUpdate(cam);

    iScrFxPostCameraEnd(cam);
}

void iCameraShowRaster(RwCamera *cam)
{
    RwCameraShowRaster(cam, NULL, FULLSCREEN ? rwRASTERFLIPWAITVSYNC : 0);
}

void iCameraFrustumPlanes(RwCamera *cam, xVec4 *frustplane)
{
    RwFrustumPlane *rwPlane = cam->frustumPlanes;

    frustplane[0].x = rwPlane[2].plane.normal.x;
    frustplane[1].x = rwPlane[2].plane.normal.y;
    frustplane[2].x = rwPlane[2].plane.normal.z;
    frustplane[3].x = rwPlane[2].plane.distance;

    frustplane[0].y = rwPlane[4].plane.normal.x;
    frustplane[1].y = rwPlane[4].plane.normal.y;
    frustplane[2].y = rwPlane[4].plane.normal.z;
    frustplane[3].y = rwPlane[4].plane.distance;

    frustplane[0].z = rwPlane[5].plane.normal.x;
    frustplane[1].z = rwPlane[5].plane.normal.y;
    frustplane[2].z = rwPlane[5].plane.normal.z;
    frustplane[3].z = rwPlane[5].plane.distance;

    frustplane[0].w = rwPlane[3].plane.normal.x;
    frustplane[1].w = rwPlane[3].plane.normal.y;
    frustplane[2].w = rwPlane[3].plane.normal.z;
    frustplane[3].w = rwPlane[3].plane.distance;

    frustplane[4].x = rwPlane[1].plane.normal.x;
    frustplane[5].x = rwPlane[1].plane.normal.y;
    frustplane[6].x = rwPlane[1].plane.normal.z;
    frustplane[7].x = rwPlane[1].plane.distance;

    frustplane[4].y = rwPlane[0].plane.normal.x;
    frustplane[5].y = rwPlane[0].plane.normal.y;
    frustplane[6].y = rwPlane[0].plane.normal.z;
    frustplane[7].y = rwPlane[0].plane.distance;

    frustplane[4].z = rwPlane[2].plane.normal.x;
    frustplane[5].z = rwPlane[2].plane.normal.y;
    frustplane[6].z = rwPlane[2].plane.normal.z;
    frustplane[7].z = rwPlane[2].plane.distance;

    frustplane[4].w = rwPlane[4].plane.normal.x;
    frustplane[5].w = rwPlane[4].plane.normal.y;
    frustplane[6].w = rwPlane[4].plane.normal.z;
    frustplane[7].w = rwPlane[4].plane.distance;

    frustplane[8].x = rwPlane[2].plane.normal.x;
    frustplane[9].x = rwPlane[2].plane.normal.y;
    frustplane[10].x = rwPlane[2].plane.normal.z;
    frustplane[11].x = rwPlane[2].plane.distance;

    frustplane[8].y = rwPlane[4].plane.normal.x;
    frustplane[9].y = rwPlane[4].plane.normal.y;
    frustplane[10].y = rwPlane[4].plane.normal.z;
    frustplane[11].y = rwPlane[4].plane.distance;

    frustplane[8].z = rwPlane[5].plane.normal.x;
    frustplane[9].z = rwPlane[5].plane.normal.y;
    frustplane[10].z = rwPlane[5].plane.normal.z;
    frustplane[11].z = rwPlane[5].plane.distance;

    frustplane[8].w = rwPlane[3].plane.normal.x;
    frustplane[9].w = rwPlane[3].plane.normal.y;
    frustplane[10].w = rwPlane[3].plane.normal.z;
    frustplane[11].w = rwPlane[3].plane.distance;
}

void iCameraUpdatePos(RwCamera *cam, xMat4x3 *pos)
{
    RwFrame *f = RwCameraGetFrame(cam);
    RwMatrix *m;

    m = RwFrameGetMatrix(f);
    xMat4x3Copy((xMat4x3 *)m, pos);

    m = RwFrameGetLTM(f);
    xMat4x3Copy((xMat4x3 *)m, pos);

    RwFrameOrthoNormalize(f);
    RwFrameUpdateObjects(f);
}

void iCameraSetFOV(RwCamera *cam, float fov)
{
    RwV2d vw;

    vw.x = itan(0.5f * DEG2RAD(fov));
    vw.y = ((float)SCREEN_HEIGHT / SCREEN_WIDTH) * vw.x;

    RwCameraSetViewWindow(cam, &vw);
}

void iCameraAssignEnv(RwCamera *camera, iEnv *env_geom)
{
    globalCamera = camera;

    RpWorldAddCamera(env_geom->world, camera);
    xShadowSetWorld(env_geom->world);
}

void iCameraSetFogParams(iFogParams *fp, float time)
{
    if (!fp || fp->type == rwFOGTYPENAFOGTYPE)
    {
        xglobals->fog.type = rwFOGTYPENAFOGTYPE;
        xglobals->fogA.type = rwFOGTYPENAFOGTYPE;
    }
    else if (time == 0.0f || fp->type != xglobals->fogA.type)
    {
        xglobals->fog = *fp;
        xglobals->fogA = *fp;

        xglobals->fog_t0 = 0;
    }
    else
    {
        xglobals->fogA = xglobals->fog;
        xglobals->fogB = *fp;

        xglobals->fog_t0 = iTimeGet();
        xglobals->fog_t1 = xglobals->fog_t0 + iTimeSecsToTicks(time);
    }
}

void iCameraUpdateFog(RwCamera *cam, iTime t)
{
    BFBBSTUB("iCameraUpdateFog");
}

void iCameraSetFogRenderStates()
{
    RwCamera *pCamera;
    iFogParams *pFogParams;
    unsigned int bite_me;

    pCamera = RwCameraGetCurrentCamera();
    pFogParams = &globals.fog;

    if (pFogParams->type == rwFOGTYPENAFOGTYPE)
    {
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FALSE);
        RwCameraSetFarClipPlane(pCamera, sCameraFarClip);
    }
    else
    {
        bite_me = RWRGBALONG(pFogParams->fogcolor.red,
                             pFogParams->fogcolor.green,
                             pFogParams->fogcolor.blue,
                             pFogParams->fogcolor.alpha);

        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void *)globals.fog.type);
        RwRenderStateSet(rwRENDERSTATEFOGCOLOR, (void *)bite_me);
        RwRenderStateSet(rwRENDERSTATEFOGDENSITY, (void *)&xglobals->fog.density);

        RwCameraSetFogDistance(pCamera, globals.fog.start);
        RwCameraSetFarClipPlane(pCamera, globals.fog.stop);
    }
}

// unofficial widescreen support
void iCameraUpdateSize(RwCamera *cam, int width, int height, float fov)
{
    BFBBSTUB("iCameraUpdateSize");
    return;

    RwRaster *raster = RwCameraGetRaster(cam);
    RwInt32 curWidth = RwRasterGetWidth(raster);
    RwInt32 curHeight = RwRasterGetHeight(raster);

    if (curWidth != width || curHeight != height)
    {
        RwV2d vw;

        RwRasterDestroy(raster);
        RwRasterDestroy(RwCameraGetZRaster(cam));
        
        RwCameraSetRaster(cam, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA));
        RwCameraSetZRaster(cam, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER));

        iCameraSetFOV(cam, fov);
    }
}