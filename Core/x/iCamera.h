#ifndef ICAMERA_H
#define ICAMERA_H

#include <rwcore.h>

RwCamera *iCameraCreate(int width, int height, int mainGameCamera);
void iCameraDestroy(RwCamera *camera);

#endif