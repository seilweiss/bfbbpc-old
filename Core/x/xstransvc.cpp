#include "xstransvc.h"

#include "xpkrsvc.h"
#include "xutil.h"

#include <string.h>
#include <stdio.h>

static int g_straninit;
static st_STRAN_DATA g_xstdata;
static st_PACKER_READ_FUNCS *g_pkrf;
static st_PACKER_ASSETTYPE *g_typeHandlers;

static void XST_reset_raw();
static st_STRAN_SCENE *XST_lock_next();
static char *XST_translate_sid_path(unsigned int sid, const char *ext);
static int XST_PreLoadScene(st_STRAN_SCENE *sdata, const char *fname);
static char *XST_translate_sid(unsigned int sid, const char *ext);
static void XST_unlock(st_STRAN_SCENE *sdata);

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

// Returns cltver
int xSTPreLoadScene(unsigned int sid, void *userdata, int flg_hiphop)
{
    int result;
    st_STRAN_SCENE *sdata;
    int cltver = 0;

    if ((flg_hiphop & 0x3) == 2)
    {
        char *fname;

        sdata = XST_lock_next();
        sdata->scnid = sid;
        sdata->userdata = userdata;
        sdata->isHOP = 1;

        fname = XST_translate_sid_path(sid, ".HOP");

        if (fname)
        {
            strcpy(sdata->fnam, fname);
            cltver = XST_PreLoadScene(sdata, fname);
        }

        if (cltver == 0)
        {
            fname = XST_translate_sid(sid, ".HOP");

            if (fname)
            {
                strcpy(sdata->fnam, fname);
                cltver = XST_PreLoadScene(sdata, fname);
            }
        }

        if (cltver == 0)
        {
            XST_unlock(sdata);
            result = 0;
        }
        else
        {
            result = cltver;
        }
    }
    else
    {
        char *fname;

        do
        {
            sdata = XST_lock_next();
            sdata->scnid = sid;
            sdata->userdata = userdata;
            sdata->isHOP = 0;

            if (sid != 'BOOT' && sid != 'FONT')
            {
                fname = XST_translate_sid_path(sid, ".HIP");

                if (fname)
                {
                    strcpy(sdata->fnam, fname);
                    cltver = XST_PreLoadScene(sdata, fname);
                }
            }

            if (cltver == 0)
            {
                fname = XST_translate_sid(sid, ".HIP");

                if (fname)
                {
                    strcpy(sdata->fnam, fname);
                    cltver = XST_PreLoadScene(sdata, fname);
                }
            }

            if (cltver == 0)
            {
                XST_unlock(sdata);
                result = 0;
            }
            else
            {
                result = cltver;
            }
        }
        while (cltver == 0);
    }

    return result;
}

// Returns cltver
static int XST_PreLoadScene(st_STRAN_SCENE *sdata, const char *fname)
{
    int cltver = 0;

    sdata->spkg = g_pkrf->Init(sdata->userdata, fname, 0x2E, &cltver, g_typeHandlers);

    if (sdata->spkg)
    {
        return cltver;
    }

    return 0;
}

// Returns BB01.HIP if sid='BB01' and ext=".HIP", for example
static char *XST_translate_sid(unsigned int sid, const char *ext)
{
    static char fname[64];

    sprintf(fname, "%s%s",
            xUtil_idtag2string(sid, 0),
            ext);

    return fname;
}

// Returns BB/BB01.HIP if sid='BB01' and ext=".HIP", for example
static char *XST_translate_sid_path(unsigned int sid, const char *ext)
{
    static char fname[64];
    
    sprintf(fname, "%c%c%s%s%s",
            xUtil_idtag2string(sid, 0)[0],
            xUtil_idtag2string(sid, 0)[1],
            "/",
            xUtil_idtag2string(sid, 0),
            ext);

    return fname;
}

static void XST_reset_raw()
{
    memset(&g_xstdata, 0, sizeof(g_xstdata));
}

static st_STRAN_SCENE *XST_lock_next()
{
    st_STRAN_SCENE *sdata = NULL;
    int i;
    int uselock;

    for (i = 0; i < 16; i++)
    {
        uselock = 1 << i;

        if (!(g_xstdata.loadlock & uselock))
        {
            g_xstdata.loadlock |= uselock;
            
            sdata = &g_xstdata.hipscn[i];
            memset(sdata, 0, sizeof(st_STRAN_SCENE));

            break;
        }
    }

    if (sdata)
    {
        sdata->lockid = i;
    }

    return sdata;
}

static void XST_unlock(st_STRAN_SCENE *sdata)
{
    int uselock;

    if (sdata)
    {
        uselock = 1 << sdata->lockid;

        if (g_xstdata.loadlock & uselock)
        {
            g_xstdata.loadlock &= ~uselock;
            memset(sdata, 0, sizeof(st_STRAN_SCENE));
        }
    }
}