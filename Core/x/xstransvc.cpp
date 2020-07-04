#include "xstransvc.h"

#include "xpkrsvc.h"
#include "xutil.h"
#include "iFile.h"
#include "iTRC.h"

#include "print.h"

#include <string.h>
#include <stdio.h>

static int g_straninit;
static st_STRAN_DATA g_xstdata;
static st_PACKER_READ_FUNCS *g_pkrf;
static st_PACKER_ASSETTYPE *g_typeHandlers;

static void XST_reset_raw();
static void XST_unlock_all();
static st_STRAN_SCENE *XST_lock_next();
static char *XST_translate_sid_path(unsigned int sid, const char *ext);
static int XST_PreLoadScene(st_STRAN_SCENE *sdata, const char *fname);
static char *XST_translate_sid(unsigned int sid, const char *ext);
static void XST_unlock(st_STRAN_SCENE *sdata);
static st_STRAN_SCENE *XST_find_bySID(unsigned int sid, int findTheHOP);
static int XST_cnt_locked();
static st_STRAN_SCENE *XST_nth_locked(int n);
static st_STRAN_SCENE *XST_get_rawinst(int n);

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

    if ((flg_hiphop & XST_SCENE_HIPHOP_MASK) == XST_SCENE_HOP)
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

int xSTQueueSceneAssets(unsigned int sid, int flg_hiphop)
{
    int result = 1;
    st_STRAN_SCENE *sdata;

    sdata = XST_find_bySID(sid,
        ((flg_hiphop & XST_SCENE_HIPHOP_MASK) == XST_SCENE_HOP) ? 1 : 0);

    if (!sdata)
    {
        result = 0;
    }
    else if (sdata->spkg)
    {
        g_pkrf->LoadLayer(sdata->spkg, PKR_LTYPE_ALL);
    }

    return result;
}

void xSTUnLoadScene(unsigned int sid, int flg_hiphop)
{
    st_STRAN_SCENE *sdata;
    int cnt;
    int i;

    if (sid == 0)
    {
        cnt = XST_cnt_locked();

        for (i = 0; i < cnt; i++)
        {
            sdata = XST_nth_locked(i);

            if (sdata->spkg)
            {
                g_pkrf->Done(sdata->spkg);
            }

            sdata->spkg = NULL;
        }

        XST_unlock_all();
    }
    else
    {
        sdata = XST_find_bySID(sid,
            ((flg_hiphop & XST_SCENE_HIPHOP_MASK) == XST_SCENE_HIP) ? 1 : 0);

        if (sdata)
        {
            if (sdata->spkg)
            {
                g_pkrf->Done(sdata->spkg);
            }

            sdata->spkg = NULL;
        }

        XST_unlock(sdata);
    }
}

float xSTLoadStep(unsigned int sid)
{
    float pct;
    int rc;

    rc = PKRLoadStep(0);

    if (rc)
    {
        pct = 0.0f;
    }
    else
    {
        pct = 1.00001f;
    }

    iTRCDisk::CheckDVDAndResetState();
    iFileAsyncService();

    return pct;
}

void xSTDisconnect(unsigned int sid, int flg_hiphop)
{
    st_STRAN_SCENE *sdata;

    sdata = XST_find_bySID(sid,
        ((flg_hiphop & XST_SCENE_HIPHOP_MASK) == XST_SCENE_HOP) ? 1 : 0);

    if (sdata)
    {
        g_pkrf->PkgDisconnect(sdata->spkg);
    }
}

int xSTSwitchScene(unsigned int sid, void *userdata,
                   int(*progmon)(void *userdata, float progress))
{
    st_STRAN_SCENE *sdata;
    int rc = 0;
    int i;

    for (i = 1; i >= 0; i--)
    {
        sdata = XST_find_bySID(sid, i);

        if (sdata)
        {
            if (progmon)
            {
                progmon(userdata, 0.0f);
            }

            rc = g_pkrf->SetActive(sdata->spkg, PKR_LTYPE_ALL);

            if (progmon)
            {
                progmon(userdata, 1.0f);
            }
        }
    }

    return rc;
}

const char *xSTAssetName(unsigned int aid)
{
    const char *aname = NULL;
    st_STRAN_SCENE *sdata;
    int scncnt = XST_cnt_locked();
    int i;

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);

        if (sdata->spkg)
        {
            aname = g_pkrf->AssetName(sdata->spkg, aid);

            if (aname)
            {
                break;
            }
        }
    }

    return aname;
}

const char *xSTAssetName(void *raw_HIP_asset)
{
    const char *aname = NULL;
    unsigned int aid;
    st_STRAN_SCENE *sdata;
    int scncnt = XST_cnt_locked();
    int i;

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        aid = PKRAssetIDFromInst(raw_HIP_asset);
        aname = g_pkrf->AssetName(sdata->spkg, aid);

        if (aname)
        {
            break;
        }
    }

    return aname;
}

void *xSTFindAsset(unsigned int aid, unsigned int *size)
{
    void *memloc = NULL;
    st_STRAN_SCENE *sdata;
    int ready;
    int scncnt;
    int i;
    int rc;

    if (!aid)
    {
        return NULL;
    }

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);

        rc = g_pkrf->PkgHasAsset(sdata->spkg, aid);

        if (rc)
        {
            memloc = g_pkrf->LoadAsset(sdata->spkg, aid, NULL, NULL);
            ready = g_pkrf->IsAssetReady(sdata->spkg, aid);

            if (!ready)
            {
                memloc = NULL;

                if (size)
                {
                    *size = 0;
                }
            }
            else
            {
                if (size)
                {
                    *size = g_pkrf->GetAssetSize(sdata->spkg, aid);
                }
            }

            break;
        }
    }

    return memloc;
}

int xSTAssetCountByType(unsigned int type)
{
    int sum = 0;
    int cnt;
    int scncnt;
    int i;
    st_STRAN_SCENE *sdata;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);

        cnt = g_pkrf->AssetCount(sdata->spkg, type);
        sum += cnt;
    }

    return sum;
}

void *xSTFindAssetByType(unsigned int type, int idx, unsigned int *size)
{
    void *memptr = NULL;
    int scncnt;
    int i;
    int sum = 0;
    int cnt;
    st_STRAN_SCENE *sdata;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        cnt = g_pkrf->AssetCount(sdata->spkg, type);

        if (idx >= sum && idx < (sum + cnt))
        {
            memptr = g_pkrf->AssetByType(sdata->spkg, type, idx - sum, size);
            break;
        }

        sum += cnt;
    }

    return memptr;
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

static void XST_unlock_all()
{
    int i;

    if (g_xstdata.loadlock)
    {
        for (i = 0; i < 16; i++)
        {
            if (g_xstdata.loadlock & (1 << i))
            {
                XST_unlock(XST_get_rawinst(i));
            }
        }
    }
}

static st_STRAN_SCENE *XST_get_rawinst(int n)
{
    return &g_xstdata.hipscn[n];
}

static int XST_cnt_locked()
{
    int i;
    int cnt = 0;

    for (i = 0; i < 16; i++)
    {
        if (g_xstdata.loadlock & (1 << i))
        {
            cnt++;
        }
    }

    return cnt;
}

static st_STRAN_SCENE *XST_nth_locked(int n)
{
    st_STRAN_SCENE *sdata = NULL;
    int i;
    int cnt = 0;
    
    for (i = 0; i < 16; i++)
    {
        if (g_xstdata.loadlock & (1 << i))
        {
            if (cnt == n)
            {
                sdata = &g_xstdata.hipscn[i];
                break;
            }

            cnt++;
        }
    }

    return sdata;
}

static st_STRAN_SCENE *XST_find_bySID(unsigned int sid, int findTheHOP)
{
    st_STRAN_SCENE *da_sdata = NULL;
    int i;

    for (i = 0; i < 16; i++)
    {
        if (g_xstdata.loadlock & (1 << i))
        {
            if (g_xstdata.hipscn[i].scnid == sid)
            {
                if ((findTheHOP || !g_xstdata.hipscn[i].isHOP) &&
                    !findTheHOP || g_xstdata.hipscn[i].isHOP)
                {
                    da_sdata = &g_xstdata.hipscn[i];
                    break;
                }
            }
        }
    }

    return da_sdata;
}