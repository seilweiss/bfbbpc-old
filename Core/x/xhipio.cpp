#include "xhipio.h"

#include <stdlib.h>
#include <string.h>

static unsigned int g_loadlock;
static st_HIPLOADDATA g_hiploadinst[8];

en_READ_ASYNC_STATUS HIPLPollRead(st_HIPLOADDATA *lddata);
void HIPLSetSpot(st_HIPLOADDATA *lddata, int spot);
int HIPLSetBypass(st_HIPLOADDATA *lddata, int enable, int use_async);
int HIPLReadString(st_HIPLOADDATA *lddata, char *buf);
int HIPLReadFloats(st_HIPLOADDATA *lddata, float *data, int cnt);
int HIPLReadLongs(st_HIPLOADDATA *lddata, int *data, int cnt);
int HIPLReadShorts(st_HIPLOADDATA *lddata, short *data, int cnt);
int HIPLReadBytes(st_HIPLOADDATA *lddata, char *data, int cnt);
void HIPLBlockExit(st_HIPLOADDATA *lddata);
unsigned int HIPLBlockEnter(st_HIPLOADDATA *lddata);
unsigned int HIPLBaseSector(st_HIPLOADDATA *lddata);
void HIPLDestroy(st_HIPLOADDATA *lddata);
st_HIPLOADDATA *HIPLCreate(const char *filename, char *dblbuf, int bufsize);

static st_HIPLOADFUNCS g_map_HIPL_funcmap =
{
    HIPLCreate,
    HIPLDestroy,
    HIPLBaseSector,
    HIPLBlockEnter,
    HIPLBlockExit,
    HIPLReadBytes,
    HIPLReadShorts,
    HIPLReadLongs,
    HIPLReadFloats,
    HIPLReadString,
    HIPLSetBypass,
    HIPLSetSpot,
    HIPLPollRead
};

st_HIPLOADFUNCS *get_HIPLFuncs()
{
    return &g_map_HIPL_funcmap;
}

st_HIPLOADDATA *HIPLCreate(const char *filename, char *dblbuf, int bufsize)
{
    st_HIPLOADDATA *lddata = NULL;
    st_FILELOADINFO *fli;
    st_HIPLOADBLOCK *tmp_blk;
    int i;
    int uselock = -1;

    for (i = 0; i < 8; i++)
    {
        if (!(g_loadlock & (1 << i)))
        {
            uselock = i;
            g_loadlock |= (1 << i);

            lddata = &g_hiploadinst[i];
            break;
        }
    }

    if (lddata)
    {
        memset(lddata, 0, sizeof(st_HIPLOADDATA));

        lddata->lockid = uselock;
        lddata->top = -1;
        lddata->base_sector = 0;
        lddata->use_async = 0;
        lddata->asyn_stat = HIP_RDSTAT_NONE;
        lddata->pos = 0;
        lddata->readTop = 0;

        for (i = 0; i < 8; i++)
        {
            tmp_blk = &lddata->stk[i];
            tmp_blk->endpos = 0;
            tmp_blk->blk_id = 0;
            tmp_blk->blk_remain = 0;
            tmp_blk->flags = 0;
        }

        fli = xBinioLoadCreate(filename);

        if (fli)
        {
            lddata->fli = fli;
            lddata->base_sector = fli->basesector;

            if (dblbuf && bufsize > 0)
            {
                fli->setDoubleBuf(fli, dblbuf, bufsize);
            }
        }
        else
        {
            HIPLDestroy(lddata);
            lddata = NULL;
        }
    }

    return lddata;
}

void HIPLDestroy(st_HIPLOADDATA *lddata)
{
    int lockid;

    if (lddata)
    {
        if (lddata->fli)
        {
            lddata->fli->destroy(lddata->fli);
        }

        lockid = lddata->lockid;

        memset(lddata, 0, sizeof(st_HIPLOADDATA));

        g_loadlock &= ~(1 << lockid);
    }
}

unsigned int HIPLBaseSector(st_HIPLOADDATA *lddata)
{
    return lddata->base_sector;
}

int HIPLSetBypass(st_HIPLOADDATA *lddata, int enable, int use_async)
{
    return 0;
}

void HIPLSetSpot(st_HIPLOADDATA *lddata, int spot)
{

}

unsigned int HIPLBlockEnter(st_HIPLOADDATA *lddata)
{
    return 0;
}

void HIPLBlockExit(st_HIPLOADDATA *lddata)
{

}

en_READ_ASYNC_STATUS HIPLPollRead(st_HIPLOADDATA *lddata)
{
    return HIP_RDSTAT_NONE;
}

int HIPLReadBytes(st_HIPLOADDATA *lddata, char *data, int cnt)
{
    return 0;
}

int HIPLReadShorts(st_HIPLOADDATA *lddata, short *data, int cnt)
{
    return 0;
}

int HIPLReadLongs(st_HIPLOADDATA *lddata, int *data, int cnt)
{
    return 0;
}

int HIPLReadFloats(st_HIPLOADDATA *lddata, float *data, int cnt)
{
    return 0;
}

int HIPLReadString(st_HIPLOADDATA *lddata, char *buf)
{
    return 0;
}