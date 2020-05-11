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

int xPadUpdate(int idx, float time_passed)
{
    BFBBSTUB("xPadUpdate");
    return 1;
}

void xPadKill()
{
    BFBBSTUB("xPadKill");
}