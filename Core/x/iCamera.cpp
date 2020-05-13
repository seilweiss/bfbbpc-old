#include "iCamera.h"

#include "iScrFX.h"
#include "zMain.h"
#include "iMath.h"
#include "xMath.h"
#include "xShadow.h"

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

void iCameraSetFOV(RwCamera *cam, float fov)
{
    RwV2d vw;

    vw.x = itan(0.5f * DEG2RAD(fov));
    vw.y = 0.75f * vw.x;

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