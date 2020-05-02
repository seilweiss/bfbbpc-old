#ifndef XORDARRAY_H
#define XORDARRAY_H

struct st_XORDEREDARRAY
{
    void **list;
    int cnt;
    int max;
    int warnlvl;
};

typedef int(*XOrdTestCallBack)(const void *vkey, void *vitem);
typedef int(*XOrdCompareCallBack)(void *vkey, void *vitem);

void XOrdInit(st_XORDEREDARRAY *array, int size, int tempAlloc);
void XOrdReset(st_XORDEREDARRAY *array);
void XOrdDone(st_XORDEREDARRAY *array, int wasTempAlloc);
void XOrdAppend(st_XORDEREDARRAY *array, void *elt);
void XOrdInsert(st_XORDEREDARRAY *array, void *elt, XOrdCompareCallBack compare);
void *XOrdRemove(st_XORDEREDARRAY *array, void *elt, int index);
int XOrdLookup(st_XORDEREDARRAY *array, const void *key, XOrdTestCallBack test);
void XOrdSort(st_XORDEREDARRAY *array, XOrdCompareCallBack test);

#endif