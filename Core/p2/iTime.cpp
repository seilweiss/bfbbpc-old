#include "iTime.h"

iTime sStartupTime;
static float sGameTime;

void iTimeInit()
{
    sStartupTime = clock();
}

void iTimeExit()
{
    return;
}

iTime iTimeGet()
{
    return clock() - sStartupTime;
}

float iTimeDiffSec(iTime time)
{
    return (float)time / CLOCKS_PER_SEC;
}

float iTimeDiffSec(iTime t0, iTime t1)
{
    return iTimeDiffSec(t1 - t0);
}

void iTimeGameAdvance(float elapsed)
{
    sGameTime += elapsed;
}

void iTimeSetGame(float time)
{
    sGameTime = time;
}