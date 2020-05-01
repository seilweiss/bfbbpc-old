#include "xbinio.h"

#include "xFile.h"

#include <stdlib.h>
#include <string.h>

struct st_BINIO_XTRADATA
{
	char *dbl_buf;
	int dblbuf_size;
	int dbl_beg;
	int dbl_end;
	int dbl_amt;
	unsigned int fpos;
	char *asyn_data;
	int asyn_amt;
	int asyn_elesize;
	int asyn_ismot;
	en_BIO_ASYNC_ERRCODES asyn_status;
	unsigned int pad[3];
	int gcaskey;
};

static unsigned int g_loadlock = 0xFFFFFF00;
static st_FILELOADINFO g_loadinst[8];
static tag_xFile g_xfload[8];
static st_BINIO_XTRADATA g_xtraload[8];
static st_BINIO_XTRADATA *g_async_context;

static void LoadDestroy(st_FILELOADINFO *fli);
static int ReadBytes(st_FILELOADINFO *fli, char *data, int count);
static int ReadMShorts(st_FILELOADINFO *fli, short *data, int count);
static int ReadMLongs(st_FILELOADINFO *fli, int *data, int count);
static int ReadMFloats(st_FILELOADINFO *fli, float *data, int count);
static int ReadMDoubles(st_FILELOADINFO *fli, double *data, int count);
static int ReadIShorts(st_FILELOADINFO *fli, short *data, int count);
static int ReadILongs(st_FILELOADINFO *fli, int *data, int count);
static int ReadIFloats(st_FILELOADINFO *fli, float *data, int count);
static int ReadIDoubles(st_FILELOADINFO *fli, double *data, int count);
static int AsyncIRead(st_FILELOADINFO *fli, int offset, char *data, int size, int n);
static int AsyncMRead(st_FILELOADINFO *fli, int offset, char *data, int size, int n);
static en_BIO_ASYNC_ERRCODES AsyncReadStatus(st_FILELOADINFO *fli);
static int SkipBytes(st_FILELOADINFO *fli, int fwd);
static int ReadSeek(st_FILELOADINFO *fli, int pos);
static void SetBuffer(st_FILELOADINFO *fli, char *dblbuffer, int bufsize);
static void DiscardBuffer(st_FILELOADINFO *fli);

static tag_xFile *BFD_open(const char *filename, const char *mode, unsigned int lockid,
						   int, void *xtradata);
static unsigned int BFD_startSector(const char *filename);
static int BFD_getLength(tag_xFile *bffp, void *xtradata);
static void BFD_close(tag_xFile *bffp, void *xtradata);
static int BFD_seek(tag_xFile *bffp, int offset, int whence, void *xtradata);
static int BFD_read(void *data, int elesize, int elecnt, tag_xFile *bffp, void *xtradata);

static int ReadRaw(st_FILELOADINFO *fli, void *data, int size, int count);
static void Swap2(char *d, int n);
static void Swap4(char *d, int n);
static void Swap8(char *d, int n);

static int BFD_AsyncRead(st_FILELOADINFO *fli, int pos, void *data, int size, int n, int);
static en_BIO_ASYNC_ERRCODES BFD_AsyncReadStatus(st_FILELOADINFO *fli);

// We'll use this for PC too, why not lol
static void BFD_cb_GCP2_readasync(tag_xFile *file);

st_FILELOADINFO *xBinioLoadCreate(const char *filename)
{
	st_FILELOADINFO *fli = NULL;
	tag_xFile *tmp_fp;
	int i;
	int uselock = -1;

	for (i = 0; i < 8; i++)
	{
		if (!(g_loadlock & (1 << i)))
		{
			uselock = i;
			g_loadlock |= (1 << i);

			fli = &g_loadinst[i];
			break;
		}
	}

	if (fli)
	{
		memset(fli, 0, sizeof(st_FILELOADINFO));

		fli->lockid = uselock & ~(uselock >> 0x1F);
		fli->xtradata = &g_xtraload[uselock];
		fli->destroy = LoadDestroy;
		fli->readBytes = ReadBytes;
		fli->readMShorts = ReadMShorts;
		fli->readMLongs = ReadMLongs;
		fli->readMFloats = ReadMFloats;
		fli->readMDoubles = ReadMDoubles;
		fli->readIShorts = ReadIShorts;
		fli->readILongs = ReadILongs;
		fli->readIFloats = ReadIFloats;
		fli->readIDoubles = ReadIDoubles;
		fli->asyncIRead = AsyncIRead;
		fli->asyncMRead = AsyncMRead;
		fli->asyncReadStatus = AsyncReadStatus;
		fli->skipBytes = SkipBytes;
		fli->seekSpot = ReadSeek;
		fli->setDoubleBuf = SetBuffer;
		fli->discardDblBuf = DiscardBuffer;
		fli->error = FIOERR_NONE;
		fli->remain = 0;
		fli->basesector = 0;
		fli->filesize = 0;
		fli->position = 0;
		fli->basesector = 0;

		tmp_fp = BFD_open(filename, "rb", uselock, 0, fli->xtradata);

		if (tmp_fp)
		{
			fli->basesector = BFD_startSector(filename);
			fli->privdata = tmp_fp;
			fli->filesize = BFD_getLength(tmp_fp, fli->xtradata);
			fli->remain = fli->filesize;
		}
		else
		{
			LoadDestroy(fli);
			fli = NULL;
		}
	}

	return fli;
}

static void LoadDestroy(st_FILELOADINFO *fli)
{
	tag_xFile *fp = (tag_xFile *)fli->privdata;
	unsigned int lockid;

	if (fp)
	{
		BFD_close(fp, fli->xtradata);
	}

	lockid = fli->lockid;

	memset(fli, 0, sizeof(st_FILELOADINFO));

	g_loadlock &= ~(1 << lockid);
}

static int SkipBytes(st_FILELOADINFO *fli, int fwd)
{
	tag_xFile *file = (tag_xFile *)fli->privdata;

	if (fli->error != FIOERR_NONE)
	{
		return 0;
	}

	if (fwd == 0)
	{
		return 1;
	}

	if (fwd < 0)
	{
		fwd = 0;
	}

	if (BFD_seek(file, fli->position + fwd, 0, fli->xtradata) != 0)
	{
		fli->error = FIOERR_SEEKFAIL;
	}
	else
	{
		fli->remain -= fwd;
		fli->position += fwd;
	}

	if (fli->error != FIOERR_NONE)
	{
		return 0;
	}
	
	return 1;
}

static int ReadSeek(st_FILELOADINFO *fli, int pos)
{
	tag_xFile *file = (tag_xFile *)fli->privdata;

	if (fli->error != FIOERR_NONE)
	{
		return 0;
	}

	if (pos >= fli->filesize)
	{
		pos = fli->filesize;
	}

	if (BFD_seek(file, pos, 0, fli->xtradata) != 0)
	{
		fli->error = FIOERR_SEEKFAIL;
	}
	else
	{
		fli->position = pos;
		fli->remain = fli->filesize - pos;
	}

	if (fli->error != FIOERR_NONE)
	{
		return 0;
	}

	return 1;
}

static void SetBuffer(st_FILELOADINFO *fli, char *dblbuffer, int bufsize)
{
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)fli->xtradata;

	if (xtra)
	{
		xtra->dbl_buf = dblbuffer;
		xtra->dblbuf_size = bufsize;
		xtra->dbl_beg = 0;
		xtra->dbl_end = 0;
		xtra->dbl_amt = 0;
	}
}

static void DiscardBuffer(st_FILELOADINFO *fli)
{
	SetBuffer(fli, NULL, 0);
}

static int ReadRaw(st_FILELOADINFO *fli, void *data, int size, int count)
{
	tag_xFile *file = (tag_xFile *)fli->privdata;
	int amt;
	int n;

	if (fli->error != FIOERR_NONE)
	{
		return 0;
	}

	if ((size * count) > fli->remain)
	{
		amt = fli->remain / size;
	}
	else
	{
		amt = count;
	}

	if (amt != 0)
	{
		if (BFD_read(data, size, amt, file, fli->xtradata) != amt)
		{
			fli->error = FIOERR_READFAIL;
		}

		n = size * amt;

		fli->remain -= n;
		fli->position += n;
	}

	return amt;
}

static int ReadBytes(st_FILELOADINFO *fli, char *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(char), count);

	return act;
}

static int ReadMShorts(st_FILELOADINFO *fli, short *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(short), count);

#ifdef _WIN32
	Swap2((char *)data, act);
#endif

	return act;
}

static int ReadMLongs(st_FILELOADINFO *fli, int *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(int), count);

#ifdef _WIN32
	Swap4((char *)data, act);
#endif

	return act;
}

static int ReadMFloats(st_FILELOADINFO *fli, float *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(float), count);

#ifdef _WIN32
	Swap4((char *)data, act);
#endif

	return act;
}

static int ReadMDoubles(st_FILELOADINFO *fli, double *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(double), count);

#ifdef _WIN32
	Swap8((char *)data, act);
#endif

	return act;
}

static int ReadIShorts(st_FILELOADINFO *fli, short *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(short), count);

#ifndef _WIN32
	Swap2((char *)data, act);
#endif

	return act;
}

static int ReadILongs(st_FILELOADINFO *fli, int *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(int), count);

#ifndef _WIN32
	Swap4((char *)data, act);
#endif

	return act;
}

static int ReadIFloats(st_FILELOADINFO *fli, float *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(float), count);

#ifndef _WIN32
	Swap4((char *)data, act);
#endif

	return act;
}

static int ReadIDoubles(st_FILELOADINFO *fli, double *data, int count)
{
	int act = ReadRaw(fli, data, sizeof(double), count);

#ifndef _WIN32
	Swap8((char *)data, act);
#endif

	return act;
}

static int AsyncMRead(st_FILELOADINFO *fli, int offset, char *data, int size, int n)
{
	return BFD_AsyncRead(fli, offset, data, size, n, 1);
}

static int AsyncIRead(st_FILELOADINFO *fli, int offset, char *data, int size, int n)
{
	return BFD_AsyncRead(fli, offset, data, size, n, 0);
}

static en_BIO_ASYNC_ERRCODES AsyncReadStatus(st_FILELOADINFO *fli)
{
	return BFD_AsyncReadStatus(fli);
}

static void Swap2(char *d, int n)
{
	unsigned short *vals = (unsigned short *)d;
	int i;

	for (i = 0; i < n; i++)
	{
		vals[i] = _byteswap_ushort(vals[i]);
	}
}

static void Swap4(char *d, int n)
{
	unsigned int *vals = (unsigned int *)d;
	int i;

	for (i = 0; i < n; i++)
	{
		vals[i] = _byteswap_ulong(vals[i]);
	}
}

static void Swap8(char *d, int n)
{
	unsigned __int64 *vals = (unsigned __int64 *)d;
	int i;

	for (i = 0; i < n; i++)
	{
		vals[i] = _byteswap_uint64(vals[i]);
	}
}

static tag_xFile *BFD_open(const char *filename, const char *mode, unsigned int lockid,
						   int, void *xtradata)
{
	tag_xFile *bffp;
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)xtradata;
	int xfflg = 0x1;

	if (strcmp(mode, "rb") == 0)
	{
		xfflg = 0x1;
		bffp = &g_xfload[lockid];
	}
	else if (strcmp(mode, "wb") == 0)
	{
		bffp = NULL;
		xfflg = 0x2;
	}
	else
	{
		bffp = NULL;
	}

	if (bffp)
	{
		strncpy(bffp->relname, filename, sizeof(bffp->relname) - 1);
		bffp->relname[sizeof(bffp->relname) - 1] = '\0';

		if (iFileOpen(filename, xfflg, bffp) != 0)
		{
			bffp = NULL;
		}
		else
		{
			xtra->fpos = 0;
			xtra->dbl_buf = NULL;
			xtra->dblbuf_size = 0;
			xtra->dbl_beg = 0;
			xtra->dbl_end = 0;
			xtra->dbl_amt = 0;
		}
	}

	return bffp;
}

static void BFD_close(tag_xFile *bffp, void *xtradata)
{
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)xtradata;

	iFileClose(bffp);

	xtra->fpos = 0;
	xtra->dbl_buf = NULL;
	xtra->dblbuf_size = 0;
	xtra->dbl_beg = 0;
	xtra->dbl_end = 0;
	xtra->dbl_amt = 0;
}

static int BFD_read(void *data, int elesize, int elecnt, tag_xFile *bffp, void *xtradata)
{
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)xtradata;
	char *dest; 
	int readbeg;
	int refill = 0;
	int remain;
	int actual = 0;
	unsigned int holdpos;
	unsigned int safety = 0;
	unsigned int numBytes = elesize * elecnt;

	if (numBytes == 0)
	{
		return 0;
	}

	if (xtra->dbl_buf &&
		xtra->dblbuf_size >= 1 &&
		numBytes <= xtra->dblbuf_size)
	{
		remain = numBytes;

		do
		{
			if (xtra->fpos < xtra->dbl_beg)
			{
				refill = 1;

				xtra->dbl_beg = xtra->fpos -
					(xtra->fpos - (xtra->fpos / xtra->dblbuf_size) * xtra->dblbuf_size);
				xtra->dbl_end = xtra->dbl_beg + xtra->dblbuf_size;
				xtra->dbl_amt = 0;
			}
			else if (xtra->fpos >= xtra->dbl_end)
			{
				refill = 1;

				xtra->dbl_beg = xtra->fpos -
					(xtra->fpos - (xtra->fpos / xtra->dblbuf_size) * xtra->dblbuf_size);
				xtra->dbl_end = xtra->dbl_beg + xtra->dblbuf_size;
				xtra->dbl_amt = 0;
			}

			if (refill)
			{
				holdpos = xtra->fpos;

				iFileSeek(bffp, xtra->dbl_beg, IFILE_SEEK_SET);

				xtra->fpos = holdpos;
				xtra->dbl_amt = iFileRead(bffp, xtra->dbl_buf, xtra->dblbuf_size);
			}

			readbeg = xtra->fpos - xtra->dbl_beg;

			if (xtra->dbl_amt > readbeg)
			{
				holdpos = xtra->dbl_amt - readbeg;

				if (remain < holdpos)
				{
					holdpos = remain;
				}

				memcpy((char *)data + actual,
					   xtra->dbl_buf + readbeg,
					   holdpos);

				actual += holdpos;
				remain -= holdpos;

				xtra->fpos += holdpos;
			}

			if (actual == numBytes || xtra->dbl_amt < 1)
			{
				break;
			}
		}
		while (safety++ <= 60000);
	}
	else
	{
		iFileSeek(bffp, xtra->fpos, IFILE_SEEK_SET);

		actual = iFileRead(bffp, data, numBytes);

		xtra->fpos += actual;
	}

	return actual / elesize;
}

static int BFD_seek(tag_xFile *bffp, int offset, int whence, void *xtradata)
{
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)xtradata;

	xtra->fpos = iFileSeek(bffp, offset, whence);

	if (xtra->fpos == -1)
	{
		return 1;
	}
	
	return 0;
}

static int BFD_getLength(tag_xFile *bffp, void *xtradata)
{
	return iFileGetSize(bffp);
}

static unsigned int BFD_startSector(const char *filename)
{
	tag_xFile file = {};
	unsigned int offset = 0;
	unsigned int length = 0;

	if (iFileFind(filename, &file) == 0)
	{
		iFileGetInfo(&file, &offset, &length);
	}

	iFileClose(&file);

	return offset;
}

static void BFD_cb_GCP2_readasync(tag_xFile *file)
{
	return;
}

static int BFD_AsyncRead(st_FILELOADINFO *fli, int pos, void *data, int size, int n, int)
{
	tag_xFile *file = (tag_xFile *)fli->privdata;
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)fli->xtradata;
	int result;
	int rc;

	if (fli->error != FIOERR_NONE)
	{
		rc = 0;
	}
	else if (g_async_context)
	{
		rc = 0;
	}
	else
	{
		g_async_context = xtra;
		
		xtra->asyn_status = BINIO_ASYNC_INPROG;
		xtra->asyn_data = (char *)data;
		xtra->asyn_elesize = size;
		xtra->asyn_amt = n;
		xtra->asyn_ismot = 1;

		iFileSeek(file, pos, IFILE_SEEK_SET);
		xFileSetUserData(file, fli);

		result = iFileReadAsync(file, data, size * n, BFD_cb_GCP2_readasync, 0);

		if (result < 0)
		{
			rc = 0;
		}
		else
		{
			xtra->gcaskey = result;
			rc = 1;
		}

		if (!rc)
		{
			g_async_context = NULL;
			xtra->asyn_status = BINIO_ASYNC_NOOP;
		}

		rc = 1;
	}

	return rc;
}

static en_BIO_ASYNC_ERRCODES BFD_AsyncReadStatus(st_FILELOADINFO *fli)
{
	en_BIO_ASYNC_ERRCODES status;
	st_BINIO_XTRADATA *xtra = (st_BINIO_XTRADATA *)fli->xtradata;
	XFILE_READSECTOR_STATUS xrdstat;
	int amtsofar;

	if (!g_async_context)
	{
		status = BINIO_ASYNC_NOOP;
	}
	else if (xtra != g_async_context)
	{
		status = BINIO_ASYNC_INPROG;
	}
	else
	{
		if (xtra->asyn_status == BINIO_ASYNC_INPROG)
		{
			amtsofar = 0;
			xrdstat = xFileReadAsyncStatus(xtra->gcaskey, &amtsofar);

			switch (xrdstat)
			{
			case XFILE_RDSTAT_INPROG:
			case XFILE_RDSTAT_QUEUED:
				xtra->asyn_status = BINIO_ASYNC_INPROG;
				break;

			case XFILE_RDSTAT_DONE:
				xtra->asyn_status = BINIO_ASYNC_DONE;
				break;

			default:
				xtra->asyn_status = BINIO_ASYNC_FAIL;
				break;
			}
		}

		status = xtra->asyn_status;

		if (status == BINIO_ASYNC_FAIL || status == BINIO_ASYNC_DONE)
		{
			xtra->asyn_status = BINIO_ASYNC_NOOP;
			g_async_context = NULL;
		}
	}

	return status;
}