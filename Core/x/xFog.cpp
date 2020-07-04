#include "xFog.h"

#include "iCamera.h"
#include "xScene.h"

#include "print.h"

void xFogClearFog()
{
    iCameraSetFogParams(NULL, 0.0f);
}

void xFogInit(void *b, void *tasset)
{
    BFBBSTUB("xFogInit");
}

void xFogUpdate(xBase *to, xScene *sc, float dt)
{
    BFBBSTUB("xFogUpdate");
}