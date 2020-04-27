#ifndef XHIPIO_H
#define XHIPIO_H

#include "xbinio.h"

enum en_READ_ASYNC_STATUS
{
	HIP_RDSTAT_NONE = 0xffffffff,
	HIP_RDSTAT_INPROG,
	HIP_RDSTAT_SUCCESS,
	HIP_RDSTAT_FAILED,
	HIP_RDSTAT_NOBYPASS,
	HIP_RDSTAT_NOASYNC
};

struct st_HIPLOADBLOCK
{
	int endpos;
	unsigned int blk_id;
	int blk_remain;
	int flags;
};

struct st_HIPLOADDATA
{
	st_FILELOADINFO *fli;
	int lockid;
	int bypass;
	int bypass_recover;
	unsigned int base_sector;
	int use_async;
	en_READ_ASYNC_STATUS asyn_stat;
	int pos;
	int top;
	int readTop;
	st_HIPLOADBLOCK stk[8];
};

struct st_HIPLOADFUNCS
{
	st_HIPLOADDATA *(*create)(const char *filename, char *dblbuf, int bufsize);
	void(*destroy)(st_HIPLOADDATA *lddata);
	unsigned int(*basesector)(st_HIPLOADDATA *lddata);
	unsigned int(*enter)(st_HIPLOADDATA *lddata);
	void(*exit)(st_HIPLOADDATA *lddata);
	int(*readBytes)(st_HIPLOADDATA *lddata, char *data, int cnt);
	int(*readShorts)(st_HIPLOADDATA *lddata, short *data, int cnt);
	int(*readLongs)(st_HIPLOADDATA *lddata, int *data, int cnt);
	int(*readFloats)(st_HIPLOADDATA *lddata, float *data, int cnt);
	int(*readString)(st_HIPLOADDATA *lddata, char *buf);
	int(*setBypass)(st_HIPLOADDATA *lddata, int enable, int use_async);
	void(*setSpot)(st_HIPLOADDATA *lddata, int spot);
	en_READ_ASYNC_STATUS(*pollRead)(st_HIPLOADDATA *lddata);
};

st_HIPLOADFUNCS *get_HIPLFuncs();

#endif