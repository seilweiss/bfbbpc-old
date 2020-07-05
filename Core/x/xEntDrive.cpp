#include "xEntDrive.h"

void xEntDriveInit(xEntDrive *drv, xEnt *driven)
{
    if (drv)
    {
        drv->flags = 0;
        drv->driven = driven;
        drv->driver = NULL;
        drv->s = 0.0f;
        drv->tm = 0.0f;
        drv->tmr = 0.0f;
        drv->odriver = NULL;
        drv->os = 0.0f;
        drv->otm = 0.0f;
        drv->otmr = 0.0f;
    }
}