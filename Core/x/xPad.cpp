#include "xPad.h"

#include "print.h"

#include <stdlib.h>
#include <string.h>

static _tagxRumble mRumbleList[32];

_tagxPad mPad[4];
_tagxPad *gDebugPad;
_tagxPad *gPlayerPad;

int xPadInit()
{
    memset(mPad, 0, sizeof(mPad));
    memset(mRumbleList, 0, sizeof(mRumbleList));

    if (!iPadInit())
    {
        return 0;
    }

    gPlayerPad = &mPad[0];

    return 1;
}

_tagxPad *xPadEnable(int idx)
{
    _tagxPad *p = &mPad[idx];

    if (p->state != 0)
    {
        return p;
    }

    if (idx != 0)
    {
        return p;
    }

    p = iPadEnable(p, idx);

    xPadRumbleEnable(idx, 1);

    return p;
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