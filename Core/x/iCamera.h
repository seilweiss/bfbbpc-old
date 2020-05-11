#ifndef ICAMERA_H
#define ICAMERA_H

#include "iFog.h"
#include "iEnv.h"

#include <rwcore.h>
#include <rpworld.h>

extern RwCamera *globalCamera;

RwCamera *iCameraCreate(int width, int height, int mainGameCamera);
void iCameraDestroy(RwCamera *camera);
void iCameraSetFOV(RwCamera *cam, float fov);
void iCameraAssignEnv(RwCamera *camera, iEnv *env_geom);
void iCameraSetFogParams(iFogParams *fp, float time);
void iCameraSetFogRenderStates();

#endif