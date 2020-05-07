#include "xPad.h"

#include "print.h"

#include <stdlib.h>

_tagxPad *gDebugPad;

int xPadInit()
{
    BFBBSTUB("xPadInit");
    return 1;
}

_tagxPad *xPadEnable(int idx)
{
    BFBBSTUB("xPadEnable");
    return NULL;
}

void xPadRumbleEnable(int idx, int enable)
{
    BFBBSTUB("xPadRumbleEnable");
}

void xPadKill()
{
    BFBBSTUB("xPadKill");
}