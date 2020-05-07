#include "xserializer.h"

#include "print.h"

static int g_serinit;

int xSerialStartup(int count, st_SERIAL_PERCID_SIZE *sizeinfo)
{
    BFBBSTUB("xSerialStartup");

    if (!g_serinit++)
    {
        // ...
    }

    return g_serinit;
}

int xSerialShutdown()
{
    g_serinit--;
    return g_serinit;
}