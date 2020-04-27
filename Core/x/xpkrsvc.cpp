#include "xpkrsvc.h"

#include "xhipio.h"

void PKR_Disconnect(st_PACKER_READ_DATA *pr);
unsigned int PKR_getPackTimestamp(st_PACKER_READ_DATA *pr);
int PKR_PkgHasAsset(st_PACKER_READ_DATA *pr, unsigned int aid);
int PKR_GetAssetInfoByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                           st_PKR_ASSET_TOCINFO *tocinfo);
int PKR_GetAssetInfo(st_PACKER_READ_DATA *pr, unsigned int aid,
                     st_PKR_ASSET_TOCINFO *tocinfo);
unsigned int PKR_GetBaseSector(st_PACKER_READ_DATA *pr);
char *PKR_AssetName(st_PACKER_READ_DATA *pr, unsigned int aid);
int PKR_SetActive(st_PACKER_READ_DATA *pr, en_LAYER_TYPE layer);
int PKR_IsAssetReady(st_PACKER_READ_DATA *pr, unsigned int aid);
int PKR_AssetCount(st_PACKER_READ_DATA *pr, unsigned int type);
void *PKR_AssetByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                      unsigned int *size);
void *PKR_LoadAsset(st_PACKER_READ_DATA *pr, unsigned int aid, char *, void *);
unsigned int PKR_GetAssetSize(st_PACKER_READ_DATA *pr, unsigned int aid);
int PKR_LoadLayer(st_PACKER_READ_DATA *, en_LAYER_TYPE);
void PKR_ReadDone(st_PACKER_READ_DATA *pr);
st_PACKER_READ_DATA *PKR_ReadInit(void *userdata, char *pkgfile, unsigned int opts,
                                  int *cltver, st_PACKER_ASSETTYPE *typelist);

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
static int pkr_sector_size;
static int g_packinit;

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

st_PACKER_READ_DATA *PKR_ReadInit(void *userdata, char *pkgfile, unsigned int opts,
                                  int *cltver, st_PACKER_ASSETTYPE *typelist)
{
    return 0;
}

void PKR_ReadDone(st_PACKER_READ_DATA *pr)
{

}

int PKR_SetActive(st_PACKER_READ_DATA *pr, en_LAYER_TYPE layer)
{
    return 0;
}

int PKR_LoadLayer(st_PACKER_READ_DATA *, en_LAYER_TYPE)
{
    return 0;
}

void *PKR_LoadAsset(st_PACKER_READ_DATA *pr, unsigned int aid, char *, void *)
{
    return 0;
}

unsigned int PKR_GetAssetSize(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    return 0;
}

int PKR_AssetCount(st_PACKER_READ_DATA *pr, unsigned int type)
{
    return 0;
}

void *PKR_AssetByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                      unsigned int *size)
{
    return 0;
}

int PKR_IsAssetReady(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    return 0;
}

unsigned int PKR_getPackTimestamp(st_PACKER_READ_DATA *pr)
{
    return 0;
}

void PKR_Disconnect(st_PACKER_READ_DATA *pr)
{

}

char *PKR_AssetName(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    return 0;
}

unsigned int PKR_GetBaseSector(st_PACKER_READ_DATA *pr)
{
    return 0;
}

int PKR_GetAssetInfo(st_PACKER_READ_DATA *pr, unsigned int aid,
                     st_PKR_ASSET_TOCINFO *tocinfo)
{
    return 0;
}

int PKR_GetAssetInfoByType(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
                           st_PKR_ASSET_TOCINFO *tocinfo)
{
    return 0;
}

int PKR_PkgHasAsset(st_PACKER_READ_DATA *pr, unsigned int aid)
{
    return 0;
}