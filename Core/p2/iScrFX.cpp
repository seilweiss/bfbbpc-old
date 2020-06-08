#include "iScrFX.h"

#include "print.h"

void iScrFxCameraCreated(RwCamera *pCamera)
{
    BFBBSTUB("iScrFxCameraCreated");
}

void iScrFxCameraEndScene(RwCamera *pCamera)
{
    BFBBSTUB("iScrFxCameraEndScene");
}

void iScrFxPostCameraEnd(RwCamera *pCamera)
{
    BFBBSTUB("iScrFxPostCameraEnd");
}

int iScrFxCameraDestroyed(RwCamera *pCamera)
{
    BFBBSTUB("iScrFxCameraDestroyed");
    return 1;
}