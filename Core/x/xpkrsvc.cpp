#include "xpkrsvc.h"

#include "xhipio.h"
#include "xMemMgr.h"
#include "xutil.h"

#include <rwcore.h>
#include <string.h>
#include <stdio.h>

enum en_PKR_LAYER_LOAD_DEST
{
    PKR_LDDEST_SKIP,
    PKR_LDDEST_KEEPSTATIC,
    PKR_LDDEST_KEEPMALLOC,
    PKR_LDDEST_RWHANDOFF,
    PKR_LDDEST_NOMORE,
    PKR_LDDEST_FORCE = RWFORCEENUMSIZEINT
};

void PKR_drv_guardLayer(st_PACKER_LTOC_NODE *layer);
en_READ_ASYNC_STATUS PKR_drv_guardVerify(st_PACKER_LTOC_NODE *layer);

static void PKR_Disconnect(st_PACKER_READ_DATA *pr);
static unsigned int PKR_getPackTimestamp(st_PACKER_READ_DATA *pr);
static int PKR_PkgHasAsset(st_PACKER_READ_DATA *pr, unsigned int aid);
static int PKR_GetAssetInfoByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                                  st_PKR_ASSET_TOCINFO *tocinfo);
static int PKR_GetAssetInfo(st_PACKER_READ_DATA *pr, unsigned int aid,
                            st_PKR_ASSET_TOCINFO *tocinfo);
static unsigned int PKR_GetBaseSector(st_PACKER_READ_DATA *pr);
static const char *PKR_AssetName(st_PACKER_READ_DATA *pr, unsigned int aid);
static int PKR_SetActive(st_PACKER_READ_DATA *pr, en_LAYER_TYPE layer);
static int PKR_IsAssetReady(st_PACKER_READ_DATA *pr, unsigned int aid);
static int PKR_AssetCount(st_PACKER_READ_DATA *pr, unsigned int type);
static void *PKR_AssetByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                             unsigned int *size);
static void *PKR_LoadAsset(st_PACKER_READ_DATA *pr, unsigned int aid, const char *, void *);
static unsigned int PKR_GetAssetSize(st_PACKER_READ_DATA *pr, unsigned int aid);
static int PKR_LoadLayer(st_PACKER_READ_DATA *, en_LAYER_TYPE);
static void PKR_ReadDone(st_PACKER_READ_DATA *pr);
static st_PACKER_READ_DATA *PKR_ReadInit(void *userdata, const char *pkgfile, 
                                         unsigned int opts, int *cltver,
                                         st_PACKER_ASSETTYPE *typelist);

static st_PACKER_READ_FUNCS g_pkr_read_funcmap_original =
{
    1,
    PKR_ReadInit,
    PKR_ReadDone,
    PKR_LoadLayer,
    PKR_GetAssetSize,
    PKR_LoadAsset,
    PKR_AssetByType,
    PKR_AssetCount,
    PKR_IsAssetReady,
    PKR_SetActive,
    PKR_AssetName,
    PKR_GetBaseSector,
    PKR_GetAssetInfo,
    PKR_GetAssetInfoByType,
    PKR_PkgHasAsset,
    PKR_getPackTimestamp,
    PKR_Disconnect
};

static st_PACKER_READ_FUNCS g_pkr_read_funcmap = g_pkr_read_funcmap_original;
static st_HIPLOADFUNCS *g_hiprf;
static st_PACKER_READ_DATA g_readdatainst[16];
static unsigned int g_loadlock;
static int pkr_sector_size;
static int g_packinit;
static int g_memalloc_pair;
static int g_memalloc_runtot;
static int g_memalloc_runfree;

static st_PACKER_ASSETTYPE *PKR_type2typeref(unsigned int type,
                                             st_PACKER_ASSETTYPE *typelist);
static int PKR_LoadStep_Async();
static void PKR_alloc_chkidx();
static void *PKR_getmem(unsigned int id, int amount, unsigned int, int align, int isTemp,
                        char **memtru);
static int PKR_parse_TOC(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static void PKR_relmem(unsigned int id, int blksize, void *memptr, unsigned int,
                       int isTemp);
static void PKR_LayerMemRelease(st_PACKER_READ_DATA *pr, st_PACKER_LTOC_NODE *layer);
static void PKR_kiilpool_anode(st_PACKER_READ_DATA *pr);
static void PKR_oldlaynode(st_PACKER_LTOC_NODE *laynode);

static int LOD_r_HIPA(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_PACK(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int PKR_makepool_anode(st_PACKER_READ_DATA *pr, int count);
static int LOD_r_DICT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static void PKR_bld_typecnt(st_PACKER_READ_DATA *pr);
static int LOD_r_STRM(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);

static int PKR_findNextLayerToLoad(st_PACKER_READ_DATA **work_on_pkg,
                                   st_PACKER_LTOC_NODE **next_layer);
static en_PKR_LAYER_LOAD_DEST PKR_layerLoadDest(en_LAYER_TYPE layer);
static char *PKR_LayerMemReserve(st_PACKER_READ_DATA *pr, st_PACKER_LTOC_NODE *layer);
static void PKR_updateLayerAssets(st_PACKER_LTOC_NODE *laynode);
static int PKR_layerTypeNeedsXForm(en_LAYER_TYPE layer);
static void PKR_xformLayerAssets(st_PACKER_LTOC_NODE *laynode);
static void *PKR_getmem(unsigned int id, int amount, unsigned int, int align);
static void PKR_push_memmark();
static void PKR_pop_memmark();
static void PKR_xform_asset(st_PACKER_ATOC_NODE *assnode, int dumpable_layer);
static int OrdTest_R_AssetID(const void *vkey, void *vitem);
static void *PKR_FindAsset(st_PACKER_READ_DATA *pr, unsigned int aid);
static int PKR_typeHdlr_idx(st_PACKER_READ_DATA *pr, unsigned int type);
static int PKR_FRIEND_assetIsGameDup(unsigned int aid, const st_PACKER_READ_DATA *skippr,
                                     int oursize, unsigned int ourtype,
                                     unsigned int chksum, char *);
static st_PACKER_ATOC_NODE *PKR_newassnode(st_PACKER_READ_DATA *pr, unsigned int aid);
static st_PACKER_LTOC_NODE *PKR_newlaynode(en_LAYER_TYPE type, int refcnt);
static int OrdComp_R_Asset(void *vkey, void *vitem);

static int LOD_r_PVER(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_PFLG(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_PCNT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_PCRT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_PMOD(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_PLAT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);

static void PKR_spew_verhist();
static int ValidatePlatform(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr, int plattag,
                            char *plat, char *vid, char *lang, char *title);

static int LOD_r_ATOC(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_LTOC(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_AINF(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_AHDR(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_ADBG(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr,
                      st_PACKER_ATOC_NODE *assnode);
static int LOD_r_LINF(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_LHDR(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_LDBG(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr,
                      st_PACKER_LTOC_NODE *laynode);
static int LOD_r_DHDR(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);
static int LOD_r_DPAK(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr);

st_PACKER_READ_FUNCS *PKRGetReadFuncs(int apiver)
{
    if (apiver == 1)
    {
        return &g_pkr_read_funcmap;
    }

    return 0;
}

int PKRStartup()
{
    if (!g_packinit++)
    {
        g_pkr_read_funcmap = g_pkr_read_funcmap_original;
        g_hiprf = get_HIPLFuncs();

        pkr_sector_size = 32;
    }

    return g_packinit;
}

int PKRShutdown()
{
    g_packinit--;
    return g_packinit;
}

int PKRLoadStep(int)
{
    int more_todo = PKR_LoadStep_Async();
    return more_todo;
}

static st_PACKER_READ_DATA *PKR_ReadInit(void *userdata, const char *pkgfile,
                                         unsigned int opts, int *cltver,
                                         st_PACKER_ASSETTYPE *typelist)
{
    st_PACKER_READ_DATA *pr = NULL;
    int i;
    int uselock;
    int lockid = -1;
    char *tocbuf_RAW = NULL;
    char *tocbuf_aligned;

    tocbuf_aligned = (char *)PKR_getmem('PTOC', 0x8000, 'PTOC', 64, 1, &tocbuf_RAW);

    for (i = 0; i < 16; i++)
    {
        uselock = 1 << i;

        if (!(g_loadlock & uselock))
        {
            g_loadlock |= uselock;
            lockid = i;
            pr = &g_readdatainst[i];

            break;
        }
    }

    if (pr)
    {
        memset(pr, 0, sizeof(st_PACKER_READ_DATA));

        pr->lockid = lockid;
        pr->userdata = userdata;
        pr->opts = opts;
        pr->types = typelist;
        pr->cltver = -1;

        strncpy(pr->packfile, pkgfile, sizeof(pr->packfile));

        if (!tocbuf_aligned)
        {
            pr->pkg = g_hiprf->create(pkgfile, NULL, 0);
        }
        else
        {
            pr->pkg = g_hiprf->create(pkgfile, tocbuf_aligned, 0x8000);
        }

        if (pr->pkg)
        {
            pr->base_sector = g_hiprf->basesector(pr->pkg);

            PKR_parse_TOC(pr->pkg, pr);

            *cltver = pr->cltver;

            g_hiprf->setBypass(pr->pkg, 1, 1);
        }
        else
        {
            PKR_ReadDone(pr);

            pr = NULL;
            *cltver = -1;
        }
    }
    else
    {
        pr = NULL;
        *cltver = -1;
    }

    PKR_relmem('PTOC', 0x8000, tocbuf_RAW, 'PTOC', 1);

    tocbuf_RAW = NULL;

    return pr;
}

static void PKR_ReadDone(st_PACKER_READ_DATA *pr)
{
    int i;
    int j;
    int lockid;
    st_PACKER_ATOC_NODE *assnode;
    st_PACKER_LTOC_NODE *laynode;
    st_XORDEREDARRAY *tmplist;

    if (pr)
    {
        for (j = pr->laytoc.cnt - 1; j >= 0; j--)
        {
            laynode = (st_PACKER_LTOC_NODE *)pr->laytoc.list[j];

            for (i = laynode->assref.cnt - 1; i >= 0; i--)
            {
                assnode = (st_PACKER_ATOC_NODE *)laynode->assref.list[i];

                if (assnode->typeref &&
                    assnode->typeref->assetUnloaded &&
                    !(assnode->loadflag & 0x100000))
                {
                    assnode->typeref->assetUnloaded(assnode->memloc, assnode->aid);
                }
            }
        }

        for (j = 0; j < pr->laytoc.cnt; j++)
        {
            laynode = (st_PACKER_LTOC_NODE *)pr->laytoc.list[j];

            if (laynode->laymem)
            {
                PKR_LayerMemRelease(pr, laynode);
                laynode->laymem = NULL;
            }
        }

        PKR_kiilpool_anode(pr);

        for (j = 0; j < pr->laytoc.cnt; j++)
        {
            laynode = (st_PACKER_LTOC_NODE *)pr->laytoc.list[j];

            PKR_oldlaynode(laynode);
        }

        XOrdDone(&pr->asstoc, 0);
        XOrdDone(&pr->laytoc, 0);

        for (i = 0; i < (sizeof(pr->typelist) / sizeof(st_XORDEREDARRAY)); i++)
        {
            tmplist = &pr->typelist[i];

            if (tmplist->max)
            {
                XOrdDone(tmplist, 0);
            }
        }

        if (pr->pkg)
        {
            g_hiprf->destroy(pr->pkg);
            pr->pkg = NULL;
        }

        lockid = pr->lockid;

        memset(pr, 0, sizeof(st_PACKER_READ_DATA));

        g_loadlock &= ~(1 << lockid);
    }
}

static int PKR_SetActive(st_PACKER_READ_DATA *pr, en_LAYER_TYPE layer)
{
    int result = 1;
    int rc;
    int i;
    int j;
    st_PACKER_ATOC_NODE *assnode;
    st_PACKER_LTOC_NODE *laynode;

    if (!pr)
    {
        rc = 0;
    }
    else
    {
        for (i = 0; i < pr->laytoc.cnt; i++)
        {
            laynode = (st_PACKER_LTOC_NODE *)pr->laytoc.list[i];

            if (layer <= PKR_LTYPE_DEFAULT || laynode->laytyp == layer)
            {
                for (j = 0; j < laynode->assref.cnt; j++)
                {
                    assnode = (st_PACKER_ATOC_NODE *)laynode->assref.list[j];

                    if (!(assnode->loadflag & 0x10000) &&
                        (assnode->loadflag & 0x80000))
                    {
                        if (!assnode->typeref)
                        {
                            // probably printing something here...
                            // WARNING: asset <name> missing typeref
                            assnode->Name();
                            xUtil_idtag2string(assnode->asstype, 0);
                        }
                        else if (assnode->typeref->assetLoaded)
                        {
                            if (!assnode->typeref->assetLoaded(pr->userdata,
                                    assnode->aid, assnode->memloc, assnode->d_size))
                            {
                                result = 0;
                            }
                            else
                            {
                                assnode->loadflag |= 0x1;
                            }
                        }
                    }
                }
            }
        }

        rc = result;
    }

    return rc;
}

static int PKR_parse_TOC(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int is_ok = 0;
    unsigned int cid;
    int done = 0;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'HIPA':
            is_ok = LOD_r_HIPA(pkg, pr);
            break;

        case 'PACK':
            LOD_r_PACK(pkg, pr);

            if (is_ok)
            {
                if (pr->asscnt > 0)
                {
                    XOrdInit(&pr->asstoc, pr->asscnt, 0);
                    PKR_makepool_anode(pr, pr->asscnt);
                }

                if (pr->laycnt > 0)
                {
                    XOrdInit(&pr->laytoc, pr->laycnt, 0);
                }
            }

            break;

        case 'DICT':
            LOD_r_DICT(pkg, pr);
            PKR_bld_typecnt(pr);

            done = 1;
            break;

        case 'STRM':
            LOD_r_STRM(pkg, pr);
            break;
        }

        if (!is_ok)
        {
            break;
        }

        g_hiprf->exit(pkg);

        if (done)
        {
            break;
        }

        cid = g_hiprf->enter(pkg);
    }

    return is_ok;
}

static int PKR_LoadStep_Async()
{
    int moretodo;
    en_READ_ASYNC_STATUS readstat;
    st_PACKER_ATOC_NODE *tmpass;
    static st_PACKER_READ_DATA *curpr = NULL;
    static st_PACKER_LTOC_NODE *asynlay = NULL;

    if (!asynlay)
    {
        PKR_findNextLayerToLoad(&curpr, &asynlay);

        if (asynlay)
        {
            en_PKR_LAYER_LOAD_DEST loaddest = PKR_layerLoadDest(asynlay->laytyp);

            if (loaddest != PKR_LDDEST_SKIP &&
                asynlay->laysize > 1 && asynlay->assref.cnt > 0)
            {
                asynlay->laymem = PKR_LayerMemReserve(curpr, asynlay);
                PKR_drv_guardLayer(asynlay);

                tmpass = (st_PACKER_ATOC_NODE *)asynlay->assref.list[0];
                g_hiprf->setSpot(tmpass->ownpkg, tmpass->d_off);

                if (g_hiprf->readBytes(tmpass->ownpkg,
                        asynlay->laymem, asynlay->laysize))
                {
                    asynlay->flg_ldstat |= 0x1000000;
                }
                else
                {
                    PKR_LayerMemRelease(curpr, asynlay);

                    asynlay->flg_ldstat &= ~0x3000000;
                    
                    curpr = NULL;
                    asynlay = NULL;
                }

                moretodo = 1;
            }
            else
            {
                asynlay->flg_ldstat |= 0x2000000;

                asynlay = NULL;
                moretodo = 1;
            }
        }
        else
        {
            curpr = NULL;
            asynlay = NULL;
            moretodo = 0;
        }
    }
    else
    {
        readstat = g_hiprf->pollRead(curpr->pkg);

        if (readstat == HIP_RDSTAT_SUCCESS)
        {
            readstat = PKR_drv_guardVerify(asynlay);
        }

        if (readstat == HIP_RDSTAT_INPROG)
        {
            moretodo = 1;
        }
        else if (readstat == HIP_RDSTAT_SUCCESS)
        {
            en_PKR_LAYER_LOAD_DEST loaddest;

            PKR_updateLayerAssets(asynlay);

            if (PKR_layerTypeNeedsXForm(asynlay->laytyp))
            {
                PKR_xformLayerAssets(asynlay);
            }

            loaddest = PKR_layerLoadDest(asynlay->laytyp);

            if (loaddest == PKR_LDDEST_RWHANDOFF)
            {
                PKR_LayerMemRelease(curpr, asynlay);
            }

            asynlay->flg_ldstat |= 0x2000000;
            asynlay = NULL;

            moretodo = 1;
        }
        else
        {
            en_PKR_LAYER_LOAD_DEST loaddest = PKR_layerLoadDest(asynlay->laytyp);

            if (asynlay->laymem && loaddest == PKR_LDDEST_RWHANDOFF)
            {
                PKR_LayerMemRelease(curpr, asynlay);
            }

            asynlay->flg_ldstat &= ~0x3000000;

            asynlay = NULL;
            curpr = NULL;

            moretodo = 1;
        }
    }

    return moretodo;
}

static char *PKR_LayerMemReserve(st_PACKER_READ_DATA *pr, st_PACKER_LTOC_NODE *layer)
{
    char *mem = NULL;
    en_PKR_LAYER_LOAD_DEST loaddest;

    if (layer->laymem)
    {
        return layer->laymem;
    }

    loaddest = PKR_layerLoadDest(layer->laytyp);

    switch (loaddest)
    {
    case PKR_LDDEST_KEEPSTATIC:
        mem = (char *)PKR_getmem('LYR\0', layer->laysize, 0x8000 + layer->laytyp, 64);
        break;

    case PKR_LDDEST_KEEPMALLOC:
        mem = (char *)PKR_getmem('LYR\0', layer->laysize, 0x8000 + layer->laytyp, 64,
                                 1, &layer->laytru);
        break;

    case PKR_LDDEST_RWHANDOFF:
        PKR_push_memmark();
        mem = (char *)PKR_getmem('LYR\0', layer->laysize, 0x8000 + layer->laytyp, 64);
        break;
    }

    return mem;
}

static void PKR_LayerMemRelease(st_PACKER_READ_DATA *pr, st_PACKER_LTOC_NODE *layer)
{
    en_PKR_LAYER_LOAD_DEST loaddest;

    loaddest = PKR_layerLoadDest(layer->laytyp);

    switch (loaddest)
    {
    case PKR_LDDEST_KEEPSTATIC:
        PKR_relmem('LYR\0', layer->laysize, layer->laymem, 0x8000 + layer->laytyp, 0);
        break;

    case PKR_LDDEST_KEEPMALLOC:
        PKR_relmem('LYR\0', layer->laysize, layer->laytru, 0x8000 + layer->laytyp, 1);
        layer->laymem = NULL;
        layer->laytru = NULL;
        break;

    case PKR_LDDEST_RWHANDOFF:
        PKR_relmem('LYR\0', layer->laysize, layer->laymem, 0x8000 + layer->laytyp, 0);
        PKR_pop_memmark();
        layer->laymem = NULL;
        break;
    }
}

void PKR_drv_guardLayer(st_PACKER_LTOC_NODE *layer)
{
    return;
}

en_READ_ASYNC_STATUS PKR_drv_guardVerify(st_PACKER_LTOC_NODE *layer)
{
    return HIP_RDSTAT_SUCCESS;
}

static en_PKR_LAYER_LOAD_DEST PKR_layerLoadDest(en_LAYER_TYPE layer)
{
    switch (layer)
    {
    case PKR_LTYPE_SRAM:
    case PKR_LTYPE_CUTSCENE:
        return PKR_LDDEST_SKIP;

    case PKR_LTYPE_TEXTURE:
    case PKR_LTYPE_BSP:
    case PKR_LTYPE_MODEL:
        return PKR_LDDEST_RWHANDOFF;

    case PKR_LTYPE_ANIMATION:
    case PKR_LTYPE_JSPINFO:
        return PKR_LDDEST_KEEPMALLOC;

    case PKR_LTYPE_DEFAULT:
    case PKR_LTYPE_VRAM:
    case PKR_LTYPE_SNDTOC:
    case PKR_LTYPE_CUTSCENETOC:
        return PKR_LDDEST_KEEPSTATIC;
    }

    return PKR_LDDEST_KEEPSTATIC;
}

static int PKR_layerTypeNeedsXForm(en_LAYER_TYPE layer)
{
    switch (layer)
    {
    case PKR_LTYPE_TEXTURE:
    case PKR_LTYPE_BSP:
    case PKR_LTYPE_MODEL:
        return 1;

    case PKR_LTYPE_DEFAULT:
    case PKR_LTYPE_ANIMATION:
    case PKR_LTYPE_VRAM:
    case PKR_LTYPE_SNDTOC:
    case PKR_LTYPE_CUTSCENETOC:
    case PKR_LTYPE_JSPINFO:
        return 1;

    case PKR_LTYPE_SRAM:
    case PKR_LTYPE_CUTSCENE:
        return 0;
    }

    return 0;
}

static int PKR_findNextLayerToLoad(st_PACKER_READ_DATA **work_on_pkg,
                                   st_PACKER_LTOC_NODE **next_layer)
{
    st_PACKER_READ_DATA *tmppr;
    st_PACKER_LTOC_NODE *tmplay;
    int i;
    int j;

    *next_layer = NULL;
    tmppr = *work_on_pkg;

    if (tmppr)
    {
        for (i = 0; i < tmppr->laytoc.cnt; i++)
        {
            tmplay = (st_PACKER_LTOC_NODE *)tmppr->laytoc.list[i];

            if (!(tmplay->flg_ldstat & 0x2000000))
            {
                *next_layer = tmplay;
                *work_on_pkg = tmppr;
                break;
            }
        }
    }

    if (!*next_layer)
    {
        for (j = 0; j < 16; j++)
        {
            tmppr = &g_readdatainst[j];

            if ((g_loadlock & (1 << j)) && tmppr != *work_on_pkg)
            {
                for (i = 0; i < tmppr->laytoc.cnt; i++)
                {
                    tmplay = (st_PACKER_LTOC_NODE *)tmppr->laytoc.list[i];

                    if (!(tmplay->flg_ldstat & 0x2000000))
                    {
                        *next_layer = tmplay;
                        *work_on_pkg = tmppr;
                        break;
                    }
                }

                if (*next_layer)
                {
                    break;
                }
            }
        }
    }

    return (*next_layer != NULL);
}

static void PKR_updateLayerAssets(st_PACKER_LTOC_NODE *laynode)
{
    int i;
    st_PACKER_ATOC_NODE *tmpass;
    int lay_hip_pos;

    tmpass = NULL;

    for (i = 0; i < laynode->assref.cnt; i++)
    {
        tmpass = (st_PACKER_ATOC_NODE *)laynode->assref.list[i];

        if (tmpass->d_off > 0 && tmpass->d_size > 0)
        {
            break;
        }

        tmpass = NULL;
    }

    if (tmpass)
    {
        lay_hip_pos = tmpass->d_off;

        for (i = 0; i < laynode->assref.cnt; i++)
        {
            tmpass = (st_PACKER_ATOC_NODE *)laynode->assref.list[i];

            if (!(tmpass->loadflag & 0x100000))
            {
                if (tmpass->loadflag & 0x200000)
                {
                    // probably printing something
                    tmpass->Name();
                    tmpass->memloc = NULL;
                }
                else
                {
                    tmpass->memloc = laynode->laymem + (tmpass->d_off - lay_hip_pos);
                    tmpass->loadflag |= 0x80000;

                    // probably printing something
                    tmpass->Name();
                }
            }
        }
    }
}

static void PKR_xformLayerAssets(st_PACKER_LTOC_NODE *laynode)
{
    int i;
    int will_be_dumped = 0;
    st_PACKER_ATOC_NODE *tmpass;
    en_PKR_LAYER_LOAD_DEST loaddest;

    loaddest = PKR_layerLoadDest(laynode->laytyp);

    if (loaddest == PKR_LDDEST_RWHANDOFF)
    {
        will_be_dumped = 1;
    }

    for (i = 0; i < laynode->assref.cnt; i++)
    {
        tmpass = (st_PACKER_ATOC_NODE *)laynode->assref.list[i];

        if (!(tmpass->loadflag & 0x100000))
        {
            PKR_xform_asset(tmpass, will_be_dumped);

            if (will_be_dumped && tmpass->x_size < 1)
            {
                // probably prints something like
                // Dumping asset <name>
                tmpass->Name();
                tmpass->memloc = NULL;
            }
        }
    }
}

static void PKR_xform_asset(st_PACKER_ATOC_NODE *assnode, int dumpable_layer)
{
    char *xformloc;
    st_PACKER_ASSETTYPE *atype;

    if (!(assnode->infoflag & 0x4))
    {
        if (assnode->typeref && assnode->typeref->readXForm)
        {
            // probably printing something
            assnode->Name();
            xUtil_idtag2string(assnode->asstype, 0);
        }
        else
        {
            return;
        }
    }

    atype = assnode->typeref;

    if (!atype)
    {
        // WARNING: asset <name> missing typeref
        assnode->Name();
    }
    else if (!atype->readXForm)
    {
        // WARNING: asset <name> missing readXForm
        assnode->Name();
    }
    else
    {
        if (assnode->d_size == 0)
        {
            // probably printing something
            assnode->Name();
            xUtil_idtag2string(assnode->asstype, 0);

            assnode->memloc = NULL;
        }

        xformloc = (char *)atype->readXForm(
            assnode->ownpr, assnode->aid, assnode->memloc, assnode->d_size,
            (unsigned int *)&assnode->x_size);

        if (!dumpable_layer && assnode->memloc == xformloc && assnode->x_size)
        {
            // probably printing something
            assnode->Name();
        }
        else if (assnode->d_size == 0 && assnode->x_size == 0)
        {
            assnode->memloc = NULL;
            assnode->loadflag |= 0x200000;
        }
        else
        {
            assnode->memloc = xformloc;
        }
    }
}

static void *PKR_FindAsset(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    int idx;
    st_PACKER_ATOC_NODE *assnode = NULL;

    idx = XOrdLookup(&pr->asstoc, (void *)aid, OrdTest_R_AssetID);

    if (idx >= 0)
    {
        assnode = (st_PACKER_ATOC_NODE *)pr->asstoc.list[idx];
    }

    if (assnode)
    {
        if (!assnode->memloc)
        {
            // WARNING: asset <name> missing data
            assnode->Name();
        }

        return assnode->memloc;
    }

    return NULL;
}

static int PKR_LoadLayer(st_PACKER_READ_DATA *, en_LAYER_TYPE)
{
    return 0;
}

static void *PKR_LoadAsset(st_PACKER_READ_DATA *pr, unsigned int aid, const char *, void *)
{
    return PKR_FindAsset(pr, aid);
}

static unsigned int PKR_GetAssetSize(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    int idx;
    st_PACKER_ATOC_NODE *assnode = NULL;

    idx = XOrdLookup(&pr->asstoc, (void *)aid, OrdTest_R_AssetID);

    if (idx > -1)
    {
        assnode = (st_PACKER_ATOC_NODE *)pr->asstoc.list[idx];
    }

    if (assnode)
    {
        if (assnode->x_size > 0)
        {
            return assnode->x_size;
        }

        return assnode->d_size;
    }

    return 0;
}

static int PKR_AssetCount(st_PACKER_READ_DATA *pr, unsigned int type)
{
    int cnt = 0;

    if (type == 0)
    {
        return pr->asstoc.cnt;
    }

    type = PKR_typeHdlr_idx(pr, type);

    if (type >= 0)
    {
        cnt = pr->typelist[type].cnt;
    }

    return cnt;
}

static void *PKR_AssetByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                             unsigned int *size)
{
    st_XORDEREDARRAY *typlist;
    st_PACKER_ATOC_NODE *assnode;

    if (size)
    {
        *size = 0;
    }

    if (idx < 0)
    {
        idx = 0;
    }

    type = PKR_typeHdlr_idx(pr, type);

    if (type < 0)
    {
        return NULL;
    }

    typlist = &pr->typelist[type];

    if (idx >= typlist->cnt)
    {
        return NULL;
    }

    assnode = (st_PACKER_ATOC_NODE *)typlist->list[idx];

    if (size)
    {
        *size = assnode->d_size;
    }

    return assnode->memloc;
}

static int PKR_IsAssetReady(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    int is_ok = 0;
    int idx;

    idx = XOrdLookup(&pr->asstoc, (void *)aid, OrdTest_R_AssetID);

    if (idx >= 0)
    {
        if (((st_PACKER_ATOC_NODE *)pr->asstoc.list[idx])->loadflag & 0x80000)
        {
            is_ok = 1;
        }
        else
        {
            is_ok = 0;
        }
    }

    return is_ok;
}

static unsigned int PKR_getPackTimestamp(st_PACKER_READ_DATA *pr)
{
    return pr->time_made;
}

static void PKR_Disconnect(st_PACKER_READ_DATA *pr)
{
    if (pr->pkg)
    {
        g_hiprf->destroy(pr->pkg);

        pr->pkg = NULL;
    }
}

unsigned int PKRAssetIDFromInst(void *asset_inst)
{
    return ((st_PACKER_ATOC_NODE *)asset_inst)->aid;
}

static const char *PKR_AssetName(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    const char *da_name = NULL;
    int idx;

    if (aid == 0)
    {
        return NULL;
    }

    idx = XOrdLookup(&pr->asstoc, (void *)aid, OrdTest_R_AssetID);

    if (idx >= 0)
    {
        da_name = ((st_PACKER_ATOC_NODE *)pr->asstoc.list[idx])->Name();
    }

    return da_name;
}

static unsigned int PKR_GetBaseSector(st_PACKER_READ_DATA *pr)
{
    return pr->base_sector;
}

static int PKR_GetAssetInfo(st_PACKER_READ_DATA *pr, unsigned int aid,
                     st_PKR_ASSET_TOCINFO *tocinfo)
{
    int idx;
    st_PACKER_ATOC_NODE *assnode;

    memset(tocinfo, 0, sizeof(st_PKR_ASSET_TOCINFO));

    idx = XOrdLookup(&pr->asstoc, (void *)aid, OrdTest_R_AssetID);

    if (idx >= 0)
    {
        assnode = (st_PACKER_ATOC_NODE *)pr->asstoc.list[idx];

        tocinfo->aid = aid;
        tocinfo->typeref = assnode->typeref;
        tocinfo->sector = pr->base_sector + assnode->d_off / pkr_sector_size;
        tocinfo->plus_offset = assnode->d_off -
            (assnode->d_off / pkr_sector_size) * pkr_sector_size;
        tocinfo->size = assnode->d_size;
        tocinfo->mempos = assnode->memloc;
    }

    return (idx >= 0);
}

static int PKR_GetAssetInfoByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                                  st_PKR_ASSET_TOCINFO *tocinfo)
{
    st_PACKER_ATOC_NODE *assnode;
    st_XORDEREDARRAY *typlist;

    memset(tocinfo, 0, sizeof(st_PKR_ASSET_TOCINFO));

    if (idx < 0)
    {
        idx = 0;
    }

    type = PKR_typeHdlr_idx(pr, type);

    if (type < 0)
    {
        return 0;
    }

    typlist = &pr->typelist[type];

    if (idx >= typlist->cnt)
    {
        return 0;
    }

    assnode = (st_PACKER_ATOC_NODE *)typlist->list[idx];

    tocinfo->aid = assnode->aid;
    tocinfo->typeref = assnode->typeref;
    tocinfo->sector = pr->base_sector + assnode->d_off / pkr_sector_size;
    tocinfo->plus_offset = assnode->d_off -
        (assnode->d_off / pkr_sector_size) * pkr_sector_size;
    tocinfo->size = assnode->d_size;
    tocinfo->mempos = assnode->memloc;

    return 1;
}

static int PKR_PkgHasAsset(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    int rc;
    int idx;

    idx = XOrdLookup(&pr->asstoc, (void *)aid, OrdTest_R_AssetID);

    if (idx < 0)
    {
        rc = 0;
    }
    else
    {
        rc = 1;

        if (((st_PACKER_ATOC_NODE *)pr->asstoc.list[idx])->loadflag & 0x100000)
        {
            rc = 0;
        }
        else if (((st_PACKER_ATOC_NODE *)pr->asstoc.list[idx])->loadflag & 0x200000)
        {
            rc = 0;
        }
    }

    return rc;
}

static int PKR_FRIEND_assetIsGameDup(unsigned int aid, const st_PACKER_READ_DATA *skippr,
                                     int oursize, unsigned int ourtype,
                                     unsigned int chksum, char *)
{
    int is_dup = 0;
    st_PACKER_ATOC_NODE *tmp_ass;
    int i;
    int idx;

    if (aid == 0x7AB6743A) // sound_info (SNDI)
    {
        return 0;
    }

    if (aid == 0x98A3F56C) // simple shadow table (SHDW)
    {
        return 0;
    }

    for (i = 0; i < 16; i++)
    {
        if ((g_loadlock & (1 << i)) && (&g_readdatainst[i] == skippr))
        {
            idx = XOrdLookup(&g_readdatainst[i].asstoc, (void *)aid, OrdTest_R_AssetID);

            if (idx >= 0)
            {
                tmp_ass = (st_PACKER_ATOC_NODE *)g_readdatainst[i].asstoc.list[idx];

                if ((tmp_ass->loadflag & 0x80000) ||
                    tmp_ass->asstype == 'SND ' ||
                    tmp_ass->asstype == 'SNDS')
                {
                    // probably printing something
                    tmp_ass->Name();
                    xUtil_idtag2string(tmp_ass->asstype, 0);

                    if (ourtype && ourtype != tmp_ass->asstype)
                    {
                        // probably printing something
                        tmp_ass->Name();
                        xUtil_idtag2string(tmp_ass->asstype, 0);
                        xUtil_idtag2string(ourtype, 1);
                    }

                    if (oursize >= 0 && oursize != tmp_ass->d_size)
                    {
                        // probably printing something
                        tmp_ass->Name();
                        xUtil_idtag2string(tmp_ass->asstype, 0);
                    }

                    if (chksum && chksum != tmp_ass->d_chksum)
                    {
                        // probably printing something
                        tmp_ass->Name();
                        xUtil_idtag2string(tmp_ass->asstype, 0);
                    }

                    is_dup = 1;
                    break;
                }
            }
        }
    }

    return is_dup;
}

static int PKR_makepool_anode(st_PACKER_READ_DATA *pr, int count)
{
    int size;
    st_PACKER_ATOC_NODE *pool;

    if (count == 0)
    {
        return 0;
    }

    size = count * sizeof(st_PACKER_ATOC_NODE);
    pool = (st_PACKER_ATOC_NODE *)PKR_getmem('ANOD', size, 'FAKE', 64);

    if (pool)
    {
        pr->pool_anode = pool;
        pr->pool_nextaidx = 0;
    }

    return size & (pool != 0);
}

static void PKR_kiilpool_anode(st_PACKER_READ_DATA *pr)
{
    int size;

    if (pr->asscnt)
    {
        size = pr->asscnt * sizeof(st_PACKER_ATOC_NODE);

        PKR_relmem('ANOD', size, pr->pool_anode, 'FAKE', 0);

        pr->pool_anode = NULL;
        pr->pool_nextaidx = 0;
    }
}

static st_PACKER_ATOC_NODE *PKR_newassnode(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    st_PACKER_ATOC_NODE *assnode = &pr->pool_anode[pr->pool_nextaidx++];

    memset(assnode, 0, sizeof(st_PACKER_ATOC_NODE));
    assnode->aid = aid;

    return assnode;
}

static st_PACKER_LTOC_NODE *PKR_newlaynode(en_LAYER_TYPE type, int refcnt)
{
    st_PACKER_LTOC_NODE *laynode;

    laynode = (st_PACKER_LTOC_NODE *)PKR_getmem('LNOD', sizeof(st_PACKER_LTOC_NODE),
                                                0x8000 + type, 64);

    memset(laynode, 0, sizeof(st_PACKER_LTOC_NODE));
    laynode->laytyp = type;

    refcnt = (refcnt > 1) ? refcnt : 2;

    XOrdInit(&laynode->assref, refcnt, 0);

    return laynode;
}

static void PKR_oldlaynode(st_PACKER_LTOC_NODE *laynode)
{
    XOrdDone(&laynode->assref, 0);
    PKR_relmem('LNOD', sizeof(st_PACKER_LTOC_NODE), laynode, 0x8000 + laynode->laytyp, 0);
}

static int OrdComp_R_Asset(void *vkey, void *vitem)
{
    int rc;

    if (((st_PACKER_ATOC_NODE *)vkey)->aid < ((st_PACKER_ATOC_NODE *)vitem)->aid)
    {
        rc = -1;
    }
    else if (((st_PACKER_ATOC_NODE *)vkey)->aid > ((st_PACKER_ATOC_NODE *)vitem)->aid)
    {
        rc = 1;
    }
    else
    {
        rc = 0;
    }

    return rc;
}

static int OrdTest_R_AssetID(const void *vkey, void *vitem)
{
    int rc;
    unsigned int key = (unsigned int)vkey;

    if (key < ((st_PACKER_ATOC_NODE *)vitem)->aid)
    {
        rc = -1;
    }
    else if (key > ((st_PACKER_ATOC_NODE *)vitem)->aid)
    {
        rc = 1;
    }
    else
    {
        rc = 0;
    }

    return rc;
}

static int LOD_r_HIPA(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    pr->pkgver = 'HIPA';
    return 1;
}

static int LOD_r_PACK(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'PVER':
            LOD_r_PVER(pkg, pr);
            break;

        case 'PFLG':
            LOD_r_PFLG(pkg, pr);
            break;

        case 'PCNT':
            LOD_r_PCNT(pkg, pr);
            break;

        case 'PCRT':
            LOD_r_PCRT(pkg, pr);
            break;

        case 'PMOD':
            LOD_r_PMOD(pkg, pr);
            break;

        case 'PLAT':
            LOD_r_PLAT(pkg, pr);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    return 1;
}

static int LOD_r_PVER(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int ver = 0;
    int amt;

    g_hiprf->readLongs(pkg, &ver, 1);
    pr->subver = ver;

    if (ver < 2)
    {
        PKR_spew_verhist();
    }

    ver = -1;

    g_hiprf->readLongs(pkg, &ver, 1);
    pr->cltver = ver;

    ver = -1;

    amt = g_hiprf->readLongs(pkg, &ver, 1);

    if (amt != 1)
    {
        pr->compatver = 1;
    }
    else
    {
        pr->compatver = ver;
    }

    return 1;
}

static int LOD_r_PFLG(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int pflg = 0;

    g_hiprf->readLongs(pkg, &pflg, 1);

    return 1;
}

static int LOD_r_PCNT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int cnt = 0;

    g_hiprf->readLongs(pkg, &cnt, 1);
    pr->asscnt = cnt;

    g_hiprf->readLongs(pkg, &cnt, 1);
    pr->laycnt = cnt;

    g_hiprf->readLongs(pkg, &cnt, 1);
    g_hiprf->readLongs(pkg, &cnt, 1);
    g_hiprf->readLongs(pkg, &cnt, 1);

    return 1;
}

static int LOD_r_PCRT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int time = 0;
    char timestr[256] = {};

    g_hiprf->readLongs(pkg, &time, 1);
    pr->time_made = time;

    if (pr->subver > 1)
    {
        g_hiprf->readString(pkg, timestr);
    }

    return 1;
}

static int LOD_r_PMOD(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int time = 0;

    g_hiprf->readLongs(pkg, &time, 1);
    pr->time_mod = time;

    return 1;
}

static int ValidatePlatform(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr, int plattag,
                            char *plat, char *vid, char *lang, char *title)
{
    char fullname[128] = {};
    int rc;

    sprintf(fullname, "%s %s %s %s", plat, vid, lang, title);

    rc = (strcmp(plat, "GameCube") == 0 ||
          strcmp(plat, "Xbox") == 0 ||
          strcmp(plat, "PlayStation 2") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(vid, "NTSC") == 0 ||
          strcmp(vid, "PAL") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(lang, "US Common") == 0 ||
          strcmp(lang, "United Kingdom") == 0 ||
          strcmp(lang, "French") == 0 ||
          strcmp(lang, "German") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(title, "Sponge Bob") == 0 ||
          strcmp(title, "Incredibles") == 0 ||
          strcmp(title, "Jimmy Newtron") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(plat, "Xbox") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(vid, "NTSC") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(lang, "US Common") == 0);

    if (!rc)
    {
        return 0;
    }

    rc = (strcmp(title, "Sponge Bob") == 0);

    if (!rc)
    {
        return 0;
    }

    return 1;
}

static int LOD_r_PLAT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int result = 1;
    int plattag = 0;
    char platname[32] = {};
    char vidname[32] = {};
    char langname[32] = {};
    char titlename[32] = {};
    int n;
    int rc;

    g_hiprf->readLongs(pkg, &plattag, 1);
    g_hiprf->readString(pkg, platname);
    g_hiprf->readString(pkg, vidname);
    g_hiprf->readString(pkg, langname);

    n = g_hiprf->readString(pkg, titlename);

    if (!n)
    {
        strcpy(titlename, "<Unknown>");
    }

    rc = ValidatePlatform(pkg, pr, plattag, platname, vidname, langname, titlename);

    if (!rc)
    {
        result = 0;
    }

    return result;
}

static int LOD_r_DICT(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'ATOC':
            LOD_r_ATOC(pkg, pr);
            XOrdSort(&pr->asstoc, OrdComp_R_Asset);
            break;

        case 'LTOC':
            LOD_r_LTOC(pkg, pr);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    return 1;
}

static int LOD_r_ATOC(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'AINF':
            LOD_r_AINF(pkg, pr);
            break;

        case 'AHDR':
            LOD_r_AHDR(pkg, pr);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    return 1;
}

static int LOD_r_AINF(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int ainf = 0;

    g_hiprf->readLongs(pkg, &ainf, 1);

    return 1;
}

static int LOD_r_AHDR(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;
    int ival = 0;
    int isdup;
    st_PACKER_ATOC_NODE *assnode;

    g_hiprf->readLongs(pkg, &ival, 1);

    assnode = PKR_newassnode(pr, ival);
    assnode->ownpr = pr;
    assnode->ownpkg = pkg;

    XOrdAppend(&pr->asstoc, assnode);

    g_hiprf->readLongs(pkg, &ival, 1);

    assnode->asstype = ival;
    assnode->typeref = PKR_type2typeref(assnode->asstype, pr->types);

    g_hiprf->readLongs(pkg, &ival, 1);

    assnode->d_off = ival;

    g_hiprf->readLongs(pkg, &ival, 1);

    assnode->d_size = ival;
    assnode->readrem = ival;
    assnode->readcnt = 0;

    if (assnode->d_size < 1)
    {
        assnode->loadflag |= 0x200000;
    }

    g_hiprf->readLongs(pkg, &ival, 1);
    assnode->d_pad = ival;

    g_hiprf->readLongs(pkg, &ival, 1);
    assnode->infoflag = ival;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'ADBG':
            LOD_r_ADBG(pkg, pr, assnode);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    isdup = PKR_FRIEND_assetIsGameDup(assnode->aid, pr, assnode->d_size,
                                      assnode->asstype, assnode->d_chksum, NULL);

    if (isdup)
    {
        assnode->loadflag |= 0x100000;
    }

    return 1;
}

static int LOD_r_ADBG(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr,
                      st_PACKER_ATOC_NODE *assnode)
{
    int ival = 0;
    char tmpbuf[256] = {};

    g_hiprf->readLongs(pkg, &ival, 1);
    assnode->assalign = ival;

    g_hiprf->readString(pkg, tmpbuf);

    tmpbuf[0] = '\0';

    g_hiprf->readString(pkg, tmpbuf);

    if (pr->subver > 1)
    {
        g_hiprf->readLongs(pkg, &ival, 1);
        assnode->d_chksum = ival;
    }

    return 1;
}

static int LOD_r_LTOC(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'LINF':
            LOD_r_LINF(pkg, pr);
            break;
            
        case 'LHDR':
            LOD_r_LHDR(pkg, pr);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    return 1;
}

static int LOD_r_LINF(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int linf = 0;

    g_hiprf->readLongs(pkg, &linf, 1);

    return 1;
}

static int LOD_r_LHDR(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;
    int ival = 0;
    en_LAYER_TYPE laytyp;
    int refcnt = 0;
    int idx;
    int i;
    st_PACKER_LTOC_NODE *laynode;
    st_PACKER_ATOC_NODE *assnode;

    g_hiprf->readLongs(pkg, &ival, 1);

    laytyp = (en_LAYER_TYPE)ival;

    g_hiprf->readLongs(pkg, &refcnt, 1);

    laynode = PKR_newlaynode(laytyp, refcnt);
    XOrdAppend(&pr->laytoc, laynode);

    for (i = 0; i < refcnt; i++)
    {
        g_hiprf->readLongs(pkg, &ival, 1);

        idx = XOrdLookup(&pr->asstoc, (void *)ival, OrdTest_R_AssetID);
        assnode = (st_PACKER_ATOC_NODE *)pr->asstoc.list[idx];

        XOrdAppend(&laynode->assref, assnode);

        if (i != (refcnt - 1))
        {
            laynode->laysize += assnode->d_size + assnode->d_pad;
        }
        else
        {
            laynode->laysize += assnode->d_size;
        }
    }

    if (laynode->laysize > 0)
    {
        laynode->laysize = (laynode->laysize + 0x7FF) & ~0x7FF;
    }

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'LDBG':
            LOD_r_LDBG(pkg, pr, laynode);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    return 1;
}

static int LOD_r_LDBG(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr,
                      st_PACKER_LTOC_NODE *laynode)
{
    int chksum = 0;

    if (pr->subver > 1)
    {
        g_hiprf->readLongs(pkg, &chksum, 1);
        laynode->chksum = chksum;
    }

    return 1;
}

static int LOD_r_STRM(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    unsigned int cid;

    cid = g_hiprf->enter(pkg);

    while (cid)
    {
        switch (cid)
        {
        case 'DHDR':
            LOD_r_DHDR(pkg, pr);
            break;

        case 'DPAK':
            LOD_r_DPAK(pkg, pr);
            break;
        }

        g_hiprf->exit(pkg);

        cid = g_hiprf->enter(pkg);
    }

    return 1;
}

static int LOD_r_DHDR(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    int dhdr = 0;

    g_hiprf->readLongs(pkg, &dhdr, 1);

    return 1;
}

static int LOD_r_DPAK(st_HIPLOADDATA *pkg, st_PACKER_READ_DATA *pr)
{
    return 1;
}

static void PKR_spew_verhist()
{
    return;
}

static st_PACKER_ASSETTYPE *PKR_type2typeref(unsigned int type,
                                             st_PACKER_ASSETTYPE *typelist)
{
    st_PACKER_ASSETTYPE *da_type = NULL;
    st_PACKER_ASSETTYPE *tmptype;

    if (typelist)
    {
        tmptype = typelist;

        while (tmptype->typetag)
        {
            if (tmptype->typetag == type)
            {
                da_type = tmptype;
                break;
            }

            tmptype++;
        }
    }

    if (!da_type)
    {
        // WARNING: asset type <type> missing typeref
        xUtil_idtag2string(type, 0);
    }

    return da_type;
}

static void PKR_bld_typecnt(st_PACKER_READ_DATA *pr)
{
    st_PACKER_LTOC_NODE *laynode;
    st_PACKER_ATOC_NODE *assnode;
    int i;
    int j;
    int idx;
    int typcnt[129] = {};
    st_XORDEREDARRAY *tmplist;
    unsigned int lasttype = 0;
    int lasttidx = 0;

    for (j = 0; j < pr->laytoc.cnt; j++)
    {
        laynode = (st_PACKER_LTOC_NODE *)pr->laytoc.list[j];

        for (i = 0; i < laynode->assref.cnt; i++)
        {
            assnode = (st_PACKER_ATOC_NODE *)laynode->assref.list[i];

            if (!(assnode->loadflag & 0x100000) &&
                !(assnode->loadflag & 0x200000))
            {
                if (lasttype && assnode->asstype == lasttype)
                {
                    idx = lasttidx;
                }
                else
                {
                    idx = PKR_typeHdlr_idx(pr, assnode->asstype);
                    lasttype = assnode->asstype;
                    lasttidx = idx;
                }

                if (idx < 0)
                {
                    xUtil_idtag2string(assnode->asstype, 0);
                    assnode->Name();

                    typcnt[128]++;
                }
                else
                {
                    typcnt[idx]++;
                }
            }
        }
    }

    for (i = 0; i < 129; i++)
    {
        tmplist = &pr->typelist[i];

        if (typcnt[i] >= 1)
        {
            XOrdInit(tmplist, (typcnt[i] > 1) ? typcnt[i] : 2, 0);
        }
    }

    for (j = 0; j < pr->laytoc.cnt; j++)
    {
        laynode = (st_PACKER_LTOC_NODE *)pr->laytoc.list[j];

        for (i = 0; i < laynode->assref.cnt; i++)
        {
            assnode = (st_PACKER_ATOC_NODE *)laynode->assref.list[i];

            if (!(assnode->loadflag & 0x100000) &&
                !(assnode->loadflag & 0x200000))
            {
                if (lasttype && assnode->asstype == lasttype)
                {
                    idx = lasttidx;
                }
                else
                {
                    idx = PKR_typeHdlr_idx(pr, assnode->asstype);
                    lasttype = assnode->asstype;
                    lasttidx = idx;
                }

                if (idx < 0)
                {
                    tmplist = &pr->typelist[128];
                }
                else
                {
                    tmplist = &pr->typelist[idx];
                }

                XOrdAppend(tmplist, assnode);
            }
        }
    }
}

static int PKR_typeHdlr_idx(st_PACKER_READ_DATA *pr, unsigned int type)
{
    int idx = 0;

    while (pr->types[idx].typetag)
    {
        if (pr->types[idx].typetag == type)
        {
            return idx;
        }

        idx++;
    }

    return -1;
}

static void PKR_alloc_chkidx()
{
    return;
}

static void *PKR_getmem(unsigned int id, int amount, unsigned int r5, int align)
{
    return PKR_getmem(id, amount, r5, align, 0, NULL);
}

static void *PKR_getmem(unsigned int id, int amount, unsigned int, int align, int isTemp,
                        char **memtru)
{
    void *memptr;

    if (amount == 0)
    {
        return 0;
    }

    if (isTemp)
    {
        memptr = xMemPushTemp(amount + align);

        if (memtru)
        {
            *memtru = (char *)memptr;
        }

        memptr = (void *)(-align & ((size_t)memptr + (align - 1)));
    }
    else
    {
        memptr = xMemAlloc(amount, align);
    }

    if (memptr)
    {
        memset(memptr, 0, amount);
    }

    g_memalloc_pair++;
    g_memalloc_runtot += amount;

    if (g_memalloc_runtot < 0)
    {
        g_memalloc_runtot = amount;
    }

    if (memptr)
    {
        xUtil_idtag2string(id, 0);
    }
    else
    {
        xUtil_idtag2string(id, 0);
    }

    return memptr;
}

static void PKR_relmem(unsigned int id, int blksize, void *memptr, unsigned int,
                       int isTemp)
{
    g_memalloc_pair--;
    g_memalloc_runfree += blksize;

    if (g_memalloc_runfree < 0)
    {
        g_memalloc_runfree = blksize;
    }

    xUtil_idtag2string(id, 0);

    if (memptr && blksize > 0)
    {
        if (isTemp)
        {
            xMemPopTemp(memptr);
        }
        else
        {
            xUtil_idtag2string(id, 1);
        }
    }
}

static void PKR_push_memmark()
{
    xMemPushBase();
}

static void PKR_pop_memmark()
{
    xMemPopBase(xMemGetBase() - 1);
}

const char *st_PACKER_ATOC_NODE::Name() const
{
    return "<unknown>";
}