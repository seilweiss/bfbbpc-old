#include "xsavegame.h"

#include "print.h"

static int g_xsginit;

int xSGStartup()
{
    BFBBSTUB("xSGStartup");

    if (!g_xsginit++)
    {
        // ...
    }

    return g_xsginit;
}

int xSGShutdown()
{
    BFBBSTUB("xSGShutdown");

    if (!--g_xsginit)
    {
        // ...
    }

    return g_xsginit;
}