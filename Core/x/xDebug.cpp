#include "xDebug.h"

#include <stdio.h>
#include <stdarg.h>

void xprintf(const char *format, ...)
{
    va_list valist;
    va_start(valist, format);
    vprintf(format, valist);
    va_end(valist);
}

void xDebugInit()
{
    return;
}

void xDebugUpdate()
{
    return;
}

void xDebugExit()
{
    return;
}

void xDebugTimestampScreen()
{
    return;
}