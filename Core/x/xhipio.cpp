#include "xhipio.h"

#include <stdlib.h>
#include <string.h>

static unsigned int g_loadlock;
static st_HIPLOADDATA g_hiploadinst[8];

static en_READ_ASYNC_STATUS HIPLPollRead(st_HIPLOADDATA *lddata);
static void HIPLSetSpot(st_HIPLOADDATA *lddata, int spot);
static int HIPLSetBypass(st_HIPLOADDATA *lddata, int enable, int use_async);
static int HIPLReadString(st_HIPLOADDATA *lddata, char *buf);
static int HIPLReadFloats(st_HIPLOADDATA *lddata, float *data, int cnt);
static int HIPLReadLongs(st_HIPLOADDATA *lddata, int *data, int cnt);
static int HIPLReadShorts(st_HIPLOADDATA *lddata, short *data, int cnt);
static int HIPLReadBytes(st_HIPLOADDATA *lddata, char *data, int cnt);
static void HIPLBlockExit(st_HIPLOADDATA *lddata);
static unsigned int HIPLBlockEnter(st_HIPLOADDATA *lddata);
static unsigned int HIPLBaseSector(st_HIPLOADDATA *lddata);
static void HIPLDestroy(st_HIPLOADDATA *lddata);
static st_HIPLOADDATA *HIPLCreate(const char *filename, char *dblbuf, int bufsize);

static int HIPLBlockRead(st_HIPLOADDATA *lddata, void *data, int cnt, int size);
static int HIPLReadAsync(st_HIPLOADDATA *lddata, int pos, char *data, int cnt, int size);
static int HIPLBypassRead(st_HIPLOADDATA *lddata, void *data, int cnt, int size);

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

static st_HIPLOADDATA *HIPLCreate(const char *filename, char *dblbuf, int bufsize)
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

static void HIPLDestroy(st_HIPLOADDATA *lddata)
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

static unsigned int HIPLBaseSector(st_HIPLOADDATA *lddata)
{
    return lddata->base_sector;
}

static int HIPLSetBypass(st_HIPLOADDATA *lddata, int enable, int use_async)
{
    lddata->fli->discardDblBuf(lddata->fli);

    if (enable && lddata->bypass)
    {
        return 0;
    }

    if (!enable && !lddata->bypass)
    {
        return 0;
    }

    if (enable)
    {
        lddata->bypass = 1;
        lddata->use_async = use_async;
        lddata->bypass_recover = lddata->fli->position;
    }
    else
    {
        lddata->fli->seekSpot(lddata->fli, lddata->bypass_recover);
        lddata->bypass = 0;
        lddata->use_async = 0;
        lddata->bypass_recover = -1;
    }

    return 1;
}

static void HIPLSetSpot(st_HIPLOADDATA *lddata, int spot)
{
    if (lddata->bypass)
    {
        lddata->pos = spot;
        lddata->fli->seekSpot(lddata->fli, spot);
    }
}

static unsigned int HIPLBlockEnter(st_HIPLOADDATA *lddata)
{
    st_HIPLOADBLOCK *top;
    unsigned int cid = 0;
    int size = 0;

    if (lddata->bypass)
    {
        return 0;
    }

    if (lddata->top >= 0 && lddata->stk[lddata->top].blk_remain <= 0)
    {
        return 0;
    }

    if (!HIPLReadLongs(lddata, (int *)&cid, -1))
    {
        cid = 0;
    }
    else
    {
        HIPLReadLongs(lddata, &size, -1);

        if (lddata->top >= 0)
        {
            lddata->stk[lddata->top].blk_remain -= size;
        }

        top = &lddata->stk[++lddata->top];

        top->blk_id = cid;
        top->blk_remain = size;
        top->endpos = lddata->pos + (top->blk_remain + (top->blk_remain & 0x1));
        top->flags = 0;
    }

    return cid;
}

static void HIPLBlockExit(st_HIPLOADDATA *lddata)
{
    st_HIPLOADBLOCK *top;

    if (!lddata->bypass)
    {
        top = &lddata->stk[lddata->top--];

        lddata->fli->skipBytes(lddata->fli, top->endpos - lddata->pos);
        lddata->pos = top->endpos;
    }
}

static int HIPLBlockRead(st_HIPLOADDATA *lddata, void *data, int cnt, int size)
{
    st_HIPLOADBLOCK *top;
    int got = 0;
    int left = 0;
    int head = 0;

    if (lddata->bypass)
    {
        return 0;
    }

    if (cnt == 0)
    {
        return 0;
    }

    if (lddata->top < 0)
    {
        top = NULL;
    }
    else
    {
        top = &lddata->stk[lddata->top];
        left = top->blk_remain / size;
    }

    if (cnt < 0)
    {
        cnt = -cnt;
        head = 1;

        if (top && cnt > left)
        {
            cnt = left;
        }
    }

    if (!head && left < cnt)
    {
        cnt = left;
    }

    if (cnt == 0)
    {
        got = 0;
    }
    else if (size == 1)
    {
        got = lddata->fli->readBytes(lddata->fli, (char *)data, cnt);
    }
    else if (size == 2)
    {
        got = lddata->fli->readMShorts(lddata->fli, (short *)data, cnt);
    }
    else if (size == 4)
    {
        got = lddata->fli->readMLongs(lddata->fli, (int *)data, cnt);
    }

    got *= size;
    lddata->pos += got;

    if (top)
    {
        top->blk_remain -= got;
    }

    return got;
}

static int HIPLBypassRead(st_HIPLOADDATA *lddata, void *data, int cnt, int size)
{
    int got = 0;

    if (!lddata->bypass)
    {
        return 0;
    }

    if (lddata->use_async)
    {
        return HIPLReadAsync(lddata, lddata->pos, (char *)data, cnt, size);
    }

    if (cnt == 0)
    {
        return 0;
    }

    // wut
    if (cnt == 0)
    {
        got = 0;
    }
    else if (size == 1)
    {
        got = lddata->fli->readBytes(lddata->fli, (char *)data, cnt);
    }
    else if (size == 2)
    {
        got = lddata->fli->readMShorts(lddata->fli, (short *)data, cnt);
    }
    else if (size == 4)
    {
        got = lddata->fli->readMLongs(lddata->fli, (int *)data, cnt);
    }

    return got * size;
}

static int HIPLReadAsync(st_HIPLOADDATA *lddata, int pos, char *data, int cnt, int size)
{
    int key;

    lddata->asyn_stat = HIP_RDSTAT_NONE;

    key = lddata->fli->asyncMRead(lddata->fli, pos, data, cnt, size);

    lddata->asyn_stat = HIP_RDSTAT_INPROG;

    return key;
}

static en_READ_ASYNC_STATUS HIPLPollRead(st_HIPLOADDATA *lddata)
{
    en_READ_ASYNC_STATUS rdstat = HIP_RDSTAT_INPROG;
    en_BIO_ASYNC_ERRCODES pollstat;

    if (!lddata->bypass)
    {
        return HIP_RDSTAT_NOBYPASS;
    }

    if (!lddata->use_async)
    {
        return HIP_RDSTAT_NOASYNC;
    }

    pollstat = lddata->fli->asyncReadStatus(lddata->fli);

    switch (pollstat)
    {
    case BINIO_ASYNC_INPROG:
        rdstat = HIP_RDSTAT_INPROG;
        break;

    case BINIO_ASYNC_DONE:
        rdstat = HIP_RDSTAT_SUCCESS;
        lddata->asyn_stat = HIP_RDSTAT_NONE;
        break;

    case BINIO_ASYNC_FAIL:
        rdstat = HIP_RDSTAT_FAILED;
        lddata->asyn_stat = HIP_RDSTAT_NONE;
        break;

    default:
        lddata->asyn_stat = HIP_RDSTAT_NONE;
    }

    return rdstat;
}

static int HIPLReadBytes(st_HIPLOADDATA *lddata, char *data, int cnt)
{
    if (lddata->bypass)
    {
        return HIPLBypassRead(lddata, data, cnt, 1);
    }
    else
    {
        return HIPLBlockRead(lddata, data, cnt, 1);
    }
}

static int HIPLReadShorts(st_HIPLOADDATA *lddata, short *data, int cnt)
{
    int got;

    if (lddata->bypass)
    {
        got = HIPLBypassRead(lddata, data, cnt, 2);
    }
    else
    {
        got = HIPLBlockRead(lddata, data, cnt, 2);
    }

    return got / 2;
}

static int HIPLReadLongs(st_HIPLOADDATA *lddata, int *data, int cnt)
{
    int got;

    if (lddata->bypass)
    {
        got = HIPLBypassRead(lddata, data, cnt, 4);
    }
    else
    {
        got = HIPLBlockRead(lddata, data, cnt, 4);
    }

    return got / 4;
}

static int HIPLReadFloats(st_HIPLOADDATA *lddata, float *data, int cnt)
{
    int got;

    if (lddata->bypass)
    {
        got = HIPLBypassRead(lddata, data, cnt, 4);
    }
    else
    {
        got = HIPLBlockRead(lddata, data, cnt, 4);
    }

    return got / 4;
}

static int HIPLReadString(st_HIPLOADDATA *lddata, char *buf)
{
    int n = 0;
    char pad = '\0';

    if (lddata->bypass)
    {
        while (true)
        {
            if (!HIPLBypassRead(lddata, &buf[n], 1, 1))
            {
                break;
            }

            if (buf[n] == '\0')
            {
                if (!(n & 0x1))
                {
                    HIPLBypassRead(lddata, &pad, 1, 1);
                }

                break;
            }

            n++;
        }
    }
    else
    {
        while (true)
        {
            if (!HIPLBlockRead(lddata, &buf[n], 1, 1))
            {
                break;
            }

            if (buf[n] == '\0')
            {
                if (!(n & 0x1))
                {
                    HIPLBlockRead(lddata, &pad, 1, 1);
                }

                break;
            }

            n++;
        }
    }

    return n;
}