#ifndef XMODELBUCKET_H
#define XMODELBUCKET_H

#include <rwcore.h>
#include <rpworld.h>

struct xModelInstance;

struct xModelBucket
{
    RpAtomic *Data;
    RpAtomic *OriginalData;
    xModelInstance *List;
    int ClipFlags;
    unsigned int PipeFlags;
};

void xModelBucket_PreCountReset();
void xModelBucket_PreCountBucket(RpAtomic *data, unsigned int pipeFlags,
                                 unsigned int subObjects);
void xModelBucket_PreCountAlloc(int maxAlphaModels);
void xModelBucket_InsertBucket(RpAtomic *data, unsigned int pipeFlags,
                               unsigned int subObjects);
void xModelBucket_Init();
void xModelBucket_Deinit();

#endif