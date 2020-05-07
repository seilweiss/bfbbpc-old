#include "xutil.h"

#include "print.h"

#include <ctype.h>

static int g_xutilinit;

int xUtilStartup()
{
    if (!g_xutilinit++)
    {
        xUtil_crc_init();
    }

    return g_xutilinit;
}

int xUtilShutdown()
{
    g_xutilinit--;
    return g_xutilinit;
}

char *xUtil_idtag2string(unsigned int srctag, int bufidx)
{
    unsigned int tag = srctag;
    char *strptr;
    char *uc;
    int l;
    char t;
    static char buf[6][10];

    if (bufidx >= 0 && bufidx < 7)
    {
        strptr = buf[bufidx];
    }
    else
    {
        strptr = buf[0];
    }

    l = 1;
    t = ((char *)&l)[sizeof(int) - 1];

    if (t != 0)
    {
        ((char *)&tag)[0] = ((char *)&srctag)[3];
        ((char *)&tag)[1] = ((char *)&srctag)[2];
        ((char *)&tag)[2] = ((char *)&srctag)[1];
        ((char *)&tag)[3] = ((char *)&srctag)[0];
    }

    uc = (char *)&tag;

    switch (bufidx)
    {
    case 4:
    case 5:
        strptr[0] = isprint(uc[0]) ? uc[0] : '?';
        strptr[1] = isprint(uc[1]) ? uc[1] : '?';
        strptr[2] = isprint(uc[2]) ? uc[2] : '?';
        strptr[3] = isprint(uc[3]) ? uc[3] : '?';
        break;
    case 6:
    default:
        strptr[0] = isprint(uc[3]) ? uc[3] : '?';
        strptr[1] = isprint(uc[2]) ? uc[2] : '?';
        strptr[2] = isprint(uc[1]) ? uc[1] : '?';
        strptr[3] = isprint(uc[0]) ? uc[0] : '?';
        break;
    }

    strptr[4] = '\0';

    if (bufidx == 6)
    {
        strptr[4] = '/';
        strptr[5] = isprint(uc[0]) ? uc[0] : '?';
        strptr[6] = isprint(uc[1]) ? uc[1] : '?';
        strptr[7] = isprint(uc[2]) ? uc[2] : '?';
        strptr[8] = isprint(uc[3]) ? uc[3] : '?';
        strptr[9] = '\0';
    }

    return strptr;
}

unsigned int xUtil_crc_init()
{
    BFBBSTUB("xUtil_crc_init");
    return 0xFFFFFFFF;
}