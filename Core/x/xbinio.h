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
    void(*destroy)(st_FILELOADINFO *fli);
    int(*readBytes)(st_FILELOADINFO *fli, char *data, int count);
    int(*readMShorts)(st_FILELOADINFO *fli, short *data, int count);
    int(*readMLongs)(st_FILELOADINFO *fli, int *data, int count);
    int(*readMFloats)(st_FILELOADINFO *fli, float *data, int count);
    int(*readMDoubles)(st_FILELOADINFO *fli, double *data, int count);
    int(*readIShorts)(st_FILELOADINFO *fli, short *data, int count);
    int(*readILongs)(st_FILELOADINFO *fli, int *data, int count);
    int(*readIFloats)(st_FILELOADINFO *fli, float *data, int count);
    int(*readIDoubles)(st_FILELOADINFO *fli, double *data, int count);
    int(*skipBytes)(st_FILELOADINFO *fli, int fwd);
    int(*seekSpot)(st_FILELOADINFO *fli, int pos);
    void(*setDoubleBuf)(st_FILELOADINFO *fli, char *dblbuffer, int bufsize);
    void(*discardDblBuf)(st_FILELOADINFO *fli);
    int(*asyncIRead)(st_FILELOADINFO *fli, int offset, char *data, int size, int n);
    int(*asyncMRead)(st_FILELOADINFO *fli, int offset, char *data, int size, int n);
    en_BIO_ASYNC_ERRCODES(*asyncReadStatus)(st_FILELOADINFO *fli);
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

st_FILELOADINFO *xBinioLoadCreate(const char *filename);

#endif