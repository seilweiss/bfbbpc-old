#include "zMenu.h"

#include "print.h"

static int sFirstBoot = 1;

void zMenuInit(unsigned int theSceneID)
{
    BFBBSTUB("zMenuInit");
}

void zMenuExit()
{
    BFBBSTUB("zMenuExit");
}

void zMenuSetup()
{
    BFBBSTUB("zMenuSetup");
}

unsigned int zMenuLoop()
{
    BFBBSTUB("zMenuLoop");
    return 0;
}

int zMenuIsFirstBoot()
{
    return sFirstBoot;
}