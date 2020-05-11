#include "xFog.h"

#include "iCamera.h"

#include "print.h"

void xFogClearFog()
{
    iCameraSetFogParams(NULL, 0.0f);
}

void xFogInit(void *b, void *tasset)
{
    BFBBSTUB("xFogInit");
}