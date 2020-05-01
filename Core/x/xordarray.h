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
void XOrdDone(st_XORDEREDARRAY *array, int wasTempAlloc);
void XOrdAppend(st_XORDEREDARRAY *array, void *elt);
int XOrdLookup(st_XORDEREDARRAY *array, void *key, XOrdTestCallBack test);
void XOrdSort(st_XORDEREDARRAY *array, XOrdCompareCallBack test);

#endif