#ifndef ICAMERA_H
#define ICAMERA_H

#include "iFog.h"
#include "iEnv.h"
#include "iTime.h"
#include "xMath3.h"

#include <rwcore.h>
#include <rpworld.h>

extern RwCamera *globalCamera;

RwCamera *iCameraCreate(int width, int height, int mainGameCamera);
void iCameraDestroy(RwCamera *camera);
void iCameraBegin(RwCamera *cam, int clear);
void iCameraEnd(RwCamera *cam);
void iCameraShowRaster(RwCamera *cam);
void iCameraFrustumPlanes(RwCamera *cam, xVec4 *frustplane);
void iCameraUpdatePos(RwCamera *cam, xMat4x3 *pos);
void iCameraSetFOV(RwCamera *cam, float fov);
void iCameraAssignEnv(RwCamera *camera, iEnv *env_geom);
void iCameraSetFogParams(iFogParams *fp, float time);
void iCameraUpdateFog(RwCamera *cam, iTime t);
void iCameraSetFogRenderStates();

#endif