#include "print.h"

#include <windows.h>

void bfbbprint(const char *str)
{
    char buf[256];
    strcpy(buf, str);
    strcat(buf, "\n");
    OutputDebugString(buf);
}

void bfbbstub(const char *funcname)
{
    char buf[256];
    strcpy(buf, "STUB: ");
    strcat(buf, funcname);
    strcat(buf, "\n");
    OutputDebugString(buf);
}