#include "iCamera.h"

#include "iScrFX.h"

#include <rwcore.h>
#include <rpworld.h>

float sCameraNearClip = 0.05f;
float sCameraFarClip = 400.0f;

static RwCamera *sMainGameCamera;

RwCamera *iCameraCreate(int width, int height, int mainGameCamera, bool scaleAspect)
{
    RwV2d vw;
    RwCamera *camera;

    camera = RwCameraCreate();

    RwCameraSetFrame(camera, RwFrameCreate());
    RwCameraSetRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA));
    RwCameraSetZRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER));
    
    RwCameraSetFarClipPlane(camera, sCameraFarClip);
    RwCameraSetNearClipPlane(camera, sCameraNearClip);
    
    vw = 1.0f;

    if (scaleAspect == true)
    {
        float aspectRatio = (float)width / (float)height;
        vw *= (aspectRatio >= 4f / 3f ? (4f / 3f) / (4f / 3f) : aspectRatio / (4f / 3f));
        vw.x *= aspectRatio / (4f / 3f);
    }

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
