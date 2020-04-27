#include "xhipio.h"

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
    return 0;
}

void HIPLDestroy(st_HIPLOADDATA *lddata)
{

}

unsigned int HIPLBaseSector(st_HIPLOADDATA *lddata)
{
    return 0;
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