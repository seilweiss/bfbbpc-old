#include "xordarray.h"

#include "xMemMgr.h"

#include <stdlib.h>

void XOrdInit(st_XORDEREDARRAY *array, int size, int tempAlloc)
{
    size = (size >= 1) ? size : 1;

    if (tempAlloc)
    {
        array->list = (void **)xMemPushTemp(size * sizeof(void *));
    }
    else
    {
        array->list = (void **)xMemAlloc(gActiveHeap, size * sizeof(void *), 0);
    }

    array->cnt = 0;
    array->max = size;
    array->warnlvl = size * 0.95f;

    if (array->warnlvl == array->max)
    {
        array->warnlvl = (array->max - 1) & ~(array->max >> 0x1F);
    }
}

void XOrdReset(st_XORDEREDARRAY *array)
{
    array->cnt = 0;
}

void XOrdDone(st_XORDEREDARRAY *array, int wasTempAlloc)
{
    if (array->max && wasTempAlloc)
    {
        xMemPopTemp(array->list);
    }

    array->list = NULL;
    array->cnt = 0;
    array->max = 0;
    array->warnlvl = 0;
}

void XOrdAppend(st_XORDEREDARRAY *array, void *elt)
{
    if (array->cnt < array->max)
    {
        array->list[array->cnt++] = elt;
    }
}

void XOrdInsert(st_XORDEREDARRAY *array, void *elt, XOrdCompareCallBack compare)
{
    int i;

    if (array->cnt < array->max)
    {
        array->cnt++;

        for (i = array->cnt - 1; i > 0; i--)
        {
            if (compare(array->list[i - 1], elt) <= 0)
            {
                array->list[i] = elt;
                return;
            }
            else
            {
                array->list[i] = array->list[i - 1];
            }
        }

        array->list[0] = elt;
    }
}

void *XOrdRemove(st_XORDEREDARRAY *array, void *elt, int index)
{
    int i;

    if (!elt)
    {
        if (index < 0)
        {
            return NULL;
        }

        if (index >= array->max)
        {
            return NULL;
        }
    }

    if (index >= 0 && index < array->max)
    {
        elt = array->list[index];
    }
    else
    {
        index = -1;

        for (i = 0; i < array->cnt; i++)
        {
            if (array->list[i] == elt)
            {
                index = i;
                break;
            }
        }
    }

    if (index < 0)
    {
        return NULL;
    }

    array->cnt--;

    for (; index < array->cnt; index++)
    {
        array->list[index] = array->list[index + 1];
    }

    return elt;
}

int XOrdLookup(st_XORDEREDARRAY *array, const void *key, XOrdTestCallBack test)
{
    int da_idx = -1;
    int k0 = 0;
    int k1 = array->cnt;
    int k;
    int v;

    while (k1 > k0)
    {
        k = (k0 + k1) / 2;
        v = test(key, array->list[k]);

        if (v == 0)
        {
            da_idx = k;
            break;
        }
        else if (v > 0)
        {
            k0 = k + 1;
        }
        else
        {
            k1 = k;
        }
    }

    return da_idx;
}

void XOrdSort(st_XORDEREDARRAY *array, XOrdCompareCallBack test)
{
    void **list = array->list;
    int num = array->cnt;
    int i;
    int j;
    int h = 1;
    void *v;

    while (h <= num)
    {
        h = h * 3 + 1;
    }

    while (h != 1)
    {
        h /= 3;

        for (i = h; i < num; i++)
        {
            v = list[i];

            for (j = i; j >= h; j -= h)
            {
                if (test(v, list[j - h]) >= 0)
                {
                    break;
                }

                list[j] = list[j - h];
            }

            list[j] = v;
        }
    }
}