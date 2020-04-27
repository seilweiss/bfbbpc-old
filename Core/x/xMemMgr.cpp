#include "xMemMgr.h"

#include "iMemMgr.h"

#include <stdlib.h>
#include <string.h>

xMemInfo_tag gMemInfo;
unsigned int gActiveHeap;
xMemHeap_tag gxHeap[3];
xMemBaseNotifyFunc sMemBaseNotifyFunc;

void xMemInit()
{
    iMemInit();

    xMemInitHeap(&gxHeap[0],
                 gMemInfo.DRAM.addr,
                 gMemInfo.DRAM.size,
                 0x8826);

    xMemInitHeap(&gxHeap[1],
                 gMemInfo.DRAM.addr + gMemInfo.DRAM.size,
                 gMemInfo.DRAM.size,
                 0x8925);

    xMemInitHeap(&gxHeap[2],
                 gMemInfo.DRAM.addr + gMemInfo.DRAM.size,
                 gMemInfo.DRAM.size,
                 0x892A);

    gxHeap[0].opp_heap[0] = 1;
    gxHeap[0].opp_heap[1] = 2;
    gxHeap[1].opp_heap[0] = 0;
    gxHeap[1].opp_heap[1] = 2;
    gxHeap[2].opp_heap[0] = 0;
    gxHeap[2].opp_heap[1] = 1;

    gActiveHeap = 0;
}

void xMemInitHeap(xMemHeap_tag *heap, unsigned int base, unsigned int size,
                  unsigned int flags)
{
    unsigned int old_base = base;
    int align = 1 << ((flags & 0x3e00) >> 9);
    xHeapState_tag *sp;

    if (flags & 0x100)
    {
        base &= -align;
        size -= old_base - base;
    }
    else
    {
        base = -align & (base + (align - 1));
        size -= base - old_base;
    }

    heap->state_idx = 0;
    heap->hard_base = base;
    heap->size = size;
    heap->flags = flags;
    heap->lastblk = 0;

    sp = &heap->state[heap->state_idx];

    sp->curr = base;
    sp->blk_ct = 0;

    if (flags & 0x10000)
    {
        heap->max_blks = 1 << ((flags & 0x4000) >> 14);
        heap->blk = (xMemBlock_tag *)malloc(heap->max_blks * sizeof(xMemBlock_tag));

        memset(heap->blk, 0, heap->max_blks * sizeof(xMemBlock_tag));
    }
    else
    {
        heap->max_blks = 0xFFFF;
        heap->blk = NULL;
    }

    heap->opp_heap[0] = -1;
    heap->opp_heap[1] = -1;
}

void xMemRegisterBaseNotifyFunc(xMemBaseNotifyFunc func)
{
    sMemBaseNotifyFunc = func;
}