#include "xMemMgr.h"

#include "iMemMgr.h"

#include <rwcore.h>

#include <stdlib.h>
#include <string.h>

struct xMemBlkInfo_tag
{
    xMemBlock_tag *header;
    unsigned int pre;
    unsigned int block;
    unsigned int post;
    unsigned int curr;
    unsigned int waste;
    unsigned int total;
};

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

void xMemExit()
{
    iMemExit();
}

void xMemInitHeap(xMemHeap_tag *heap, unsigned int base, unsigned int size,
                  unsigned int flags)
{
    unsigned int old_base = base;
    int align = xMemHeapFlagsGetAlign(flags);
    xHeapState_tag *sp;

    if (flags & XMEMHEAP_UNK100)
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

    if (flags & XMEMHEAP_UNK10000)
    {
        heap->max_blks = xMemHeapFlagsGetMaxBlks(flags);
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

static unsigned int xMemGetBlockInfo(xMemHeap_tag *heap, unsigned int size, int align,
                                     xMemBlkInfo_tag *info)
{
    int total;
    int hdr;
    int pre;
    int block;
    int post;
    xHeapState_tag *sp = &heap->state[heap->state_idx];

    int r0 = 1;

    if (heap->flags & XMEMHEAP_UNK100)
    {
        r0 = -1;
    }

    int r10 = 0xC & -(int)xMemHeapFlagsGetUnk8000(heap->flags);

    if (heap->flags & XMEMHEAP_UNK100)
    {
        hdr = -r10;
        pre = -(int)(r10 + size + ((align - 1) & (sp->curr - r10 - size)));
        post = pre + size;
        block = -pre;
    }
    else
    {
        int remainder = ((align - 1) & (sp->curr + r10));

        if (remainder == 0)
        {
            remainder = align;
        }

        hdr = 0;
        pre = align + r10 - remainder;
        post = pre + size;
        block = post;
    }

    total = (block + 3) & ~3;

    if (heap->flags & XMEMHEAP_UNK10000)
    {
        info->header = &heap->blk[sp->blk_ct];
    }
    else
    {
        info->header = (xMemBlock_tag *)(sp->curr + hdr);
    }

    info->pre = sp->curr + pre;
    info->block = sp->curr + pre;
    info->post = sp->curr + post;
    info->curr = sp->curr + r0 * total;
    info->total = total;

    return total;
}

void *xMemGrowAlloc(unsigned int heapID, unsigned int size)
{
    size = size + 3 & ~3;

    xMemHeap_tag *heap = &gxHeap[heapID];
    xMemBlock_tag *hdr = heap->lastblk;
    xHeapState_tag *sp = &heap->state[heap->state_idx];
    void *memptr;

    unsigned int r7 = hdr->size + size - hdr->size;

    if (sp->used + r7 > heap->size)
    {
        return NULL;
    }

    if (heap->flags & XMEMHEAP_UNK100)
    {
        hdr->addr -= size;
        memptr = (void *)hdr->addr;

        sp->curr -= r7;
    }
    else
    {
        sp->curr += r7;
        memptr = (void *)(hdr->addr + hdr->size);
    }

    sp->used += r7;
    hdr->size += size;

    memset(memptr, 0, size);

    return memptr;
}

void *xMemGrowAlloc(unsigned int size)
{
    return xMemGrowAlloc(gActiveHeap, size);
}

void *xMemAlloc(unsigned int heapID, unsigned int size, int align)
{
    xMemHeap_tag *heap = &gxHeap[heapID];
    xMemBlock_tag *hdr;
    xHeapState_tag *sp = &heap->state[heap->state_idx];
    xMemBlkInfo_tag info;
    unsigned int total;

    if (align <= xMemHeapFlagsGetAlign(heap->flags))
    {
        align = xMemHeapFlagsGetAlign(heap->flags);
    }

    if (size == 0)
    {
        return (void *)0xDEADBEEF;
    }

    total = xMemGetBlockInfo(heap, size, align, &info);
    hdr = info.header;

    if ((sp->used + total) > heap->size)
    {
        return NULL;
    }

    sp->curr = info.curr;
    sp->blk_ct++;

    if (heap->flags & XMEMHEAP_UNK10000)
    {
        return NULL;
    }

    hdr->addr = info.block;
    hdr->size = size;
    hdr->align = align;

    sp->used += total;
    sp->wasted += info.waste;

    memset((void *)hdr->addr, 0, size);

    heap->lastblk = hdr;

    return (void *)hdr->addr;
}

void *xMemAlloc(unsigned int size, int align)
{
    return xMemAlloc(gActiveHeap, size, align);
}

void *xMemPushTemp(unsigned int size)
{
    void *addr;

    addr = RwMalloc(size, rwMEMHINTDUR_FUNCTION);

    return addr;
}

void xMemPopTemp(void *addr)
{
    RwFree(addr);
}

int xMemPushBase(unsigned int heapID)
{
    xMemHeap_tag *heap = &gxHeap[heapID];

    heap->state_idx++;
    heap->state[heap->state_idx] = heap->state[heap->state_idx - 1];

    if (sMemBaseNotifyFunc)
    {
        sMemBaseNotifyFunc();
    }

    return heap->state_idx - 1;
}

int xMemPushBase()
{
    return xMemPushBase(gActiveHeap);
}

int xMemPopBase(unsigned int heapID, int depth)
{
    xMemHeap_tag *heap = &gxHeap[heapID];

    if (depth < 0)
    {
        depth += heap->state_idx;
    }

    heap->state_idx = depth;

    if (sMemBaseNotifyFunc)
    {
        sMemBaseNotifyFunc();
    }

    return heap->state_idx;
}

int xMemPopBase(int depth)
{
    return xMemPopBase(gActiveHeap, depth);
}

int xMemGetBase(unsigned int heapID)
{
    return gxHeap[heapID].state_idx;
}

void xMemRegisterBaseNotifyFunc(xMemBaseNotifyFunc func)
{
    sMemBaseNotifyFunc = func;
}

int xMemGetBase()
{
    return xMemGetBase(gActiveHeap);
}

static void xMemPoolAddElements(xMemPool *pool, void *buffer, unsigned int count)
{
    int i;
    void *curr = buffer;
    xMemPoolInitCallBack initCB = pool->InitCB;
    unsigned int next = pool->NextOffset;
    unsigned int size = pool->Size;

    for (i = 0; i < count - 1; i++)
    {
        *((size_t *)((char *)curr + next)) = (size_t)((char *)curr + size);

        if (initCB)
        {
            initCB(pool, curr);
        }

        curr = (char *)curr + size;
    }

    *(void **)((char *)curr + next) = pool->FreeList;

    if (initCB)
    {
        initCB(pool, curr);
    }

    pool->FreeList = buffer;
    pool->Total += count;
}

void xMemPoolSetup(xMemPool *pool, void *buffer, unsigned int nextOffset,
                   unsigned int flags, xMemPoolInitCallBack initCB, unsigned int size,
                   unsigned int count, unsigned int numRealloc)
{
    pool->FreeList = NULL;
    pool->NextOffset = nextOffset;
    pool->Flags = flags;
    pool->UsedList = NULL;
    pool->InitCB = initCB;
    pool->Buffer = buffer;
    pool->Size = size;
    pool->NumRealloc = numRealloc;
    pool->Total = 0;

    xMemPoolAddElements(pool, buffer, count);
}