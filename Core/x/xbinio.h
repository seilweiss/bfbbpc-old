#ifndef XBINIO_H
#define XBINIO_H

enum en_BIO_ASYNC_ERRCODES
{
	BINIO_ASYNC_FAIL = 0xffffffff,
	BINIO_ASYNC_NOOP,
	BINIO_ASYNC_INPROG,
	BINIO_ASYNC_DONE,
	BINIO_ASYNC_FORCEENUMSIZEINT = 0x7fffffff
};

enum en_FIOERRCODES
{
	FIOERR_NONE,
	FIOERR_READFAIL,
	FIOERR_WRITEFAIL,
	FIOERR_SEEKFAIL,
	FIOERR_USERABORT
};

struct st_FILELOADINFO
{
	void(*destroy)(st_FILELOADINFO *);
	int(*readBytes)(st_FILELOADINFO *, char *, int);
	int(*readMShorts)(st_FILELOADINFO *, short *, int);
	int(*readMLongs)(st_FILELOADINFO *, int *, int);
	int(*readMFloats)(st_FILELOADINFO *, float *, int);
	int(*readMDoubles)(st_FILELOADINFO *, double *, int);
	int(*readIShorts)(st_FILELOADINFO *, short *, int);
	int(*readILongs)(st_FILELOADINFO *, int *, int);
	int(*readIFloats)(st_FILELOADINFO *, float *, int);
	int(*readIDoubles)(st_FILELOADINFO *, double *, int);
	int(*skipBytes)(st_FILELOADINFO *, int);
	int(*seekSpot)(st_FILELOADINFO *, int);
	void(*setDoubleBuf)(st_FILELOADINFO *, char *, int);
	void(*discardDblBuf)(st_FILELOADINFO *);
	int(*asyncIRead)(st_FILELOADINFO *, int, char *, int, int);
	int(*asyncMRead)(st_FILELOADINFO *, int, char *, int, int);
	en_BIO_ASYNC_ERRCODES(*asyncReadStatus)(st_FILELOADINFO *);
	unsigned int lockid;
	en_FIOERRCODES error;
	unsigned int basesector;
	void *privdata;
	void *xtradata;
	void *asyndata;
	int filesize;
	int remain;
	int position;
};

#endif