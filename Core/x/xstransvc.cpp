#include "xstransvc.h"

#include "xpkrsvc.h"

#include <string.h>

static int g_straninit;
static st_STRAN_DATA g_xstdata;
static st_PACKER_READ_FUNCS *g_pkrf;
static st_PACKER_ASSETTYPE *g_typeHandlers;

static void XST_reset_raw();

int xSTStartup(st_PACKER_ASSETTYPE *handlers)
{
    if (!g_straninit++)
    {
        g_typeHandlers = handlers;

        PKRStartup();
        g_pkrf = PKRGetReadFuncs(1);
    }

    return g_straninit;
}

static void XST_reset_raw()
{
    memset(&g_xstdata, 0, sizeof(g_xstdata));
}