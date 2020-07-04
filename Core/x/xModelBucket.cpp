#include "xModelBucket.h"

#include "print.h"

static int sBucketClipCullCount;
static xModelBucket **sBucketClipCullList;

void xModelBucket_PreCountReset()
{
    BFBBSTUB("xModelBucket_PreCountReset");
}

void xModelBucket_PreCountBucket(RpAtomic *data, unsigned int pipeFlags,
                                 unsigned int subObjects)
{
    BFBBSTUB("xModelBucket_PreCountBucket");
}

void xModelBucket_PreCountAlloc(int maxAlphaModels)
{
    BFBBSTUB("xModelBucket_PreCountAlloc");
}

void xModelBucket_InsertBucket(RpAtomic *data, unsigned int pipeFlags,
                               unsigned int subObjects)
{
    BFBBSTUB("xModelBucket_InsertBucket");
}

void xModelBucket_Init()
{
    BFBBSTUB("xModelBucket_Init");
}

xModelBucket **xModelBucket_GetBuckets(RpAtomic *data)
{
    int i;

    for (i = 0; i < sBucketClipCullCount; i += 2)
    {
        if (sBucketClipCullList[i]->OriginalData == data)
        {
            return &sBucketClipCullList[i];
        }
    }

    return NULL;
}

void xModelBucket_Begin()
{
    BFBBSTUB("xModelBucket_Begin");
}

void xModelBucket_RenderOpaque()
{
    BFBBSTUB("xModelBucket_RenderOpaque");
}

void xModelBucket_RenderAlpha()
{
    BFBBSTUB("xModelBucket_RenderAlpha");
}

void xModelBucket_Deinit()
{
    BFBBSTUB("xModelBucket_Deinit");
}