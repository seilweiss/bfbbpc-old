#ifndef XMEMMGR_H
#define XMEMMGR_H

struct xHeapState_tag
{
    unsigned int curr;
    unsigned short blk_ct;
    unsigned short pad;
    unsigned int used;
    unsigned int wasted;
};

struct xMemArea_tag
{
    size_t addr;
    size_t size;
    unsigned int flags;
};

struct xMemInfo_tag
{
    xMemArea_tag system;
    xMemArea_tag stack;
    xMemArea_tag DRAM;
    xMemArea_tag SRAM;
};

struct xMemBlock_tag
{
    unsigned int addr;
    unsigned int size;
    int align;
};

struct xMemHeap_tag
{
    unsigned int flags;
    unsigned int hard_base;
    unsigned int size;
    short opp_heap[2];
    xHeapState_tag state[12];
    unsigned short state_idx;
    unsigned short max_blks;
    xMemBlock_tag *blk;
    xMemBlock_tag *lastblk;
};

#define XMEMHEAP_UNK100   0x100
#define XMEMHEAP_UNK10000 0x10000

#define xMemHeapFlagsGetAlign(flags) (1 << (((flags) & 0x3e00) >> 9))
#define xMemHeapFlagsGetMaxBlks(flags) (1 << (((flags) & 0x4000) >> 14))
#define xMemHeapFlagsGetUnk8000(flags) (((flags) & 0x8000) >> 15)

struct xMemPool;

typedef void(*xMemPoolInitCallBack)(xMemPool *, void *);

struct xMemPool
{
    void *FreeList;
    unsigned short NextOffset;
    unsigned short Flags;
    void *UsedList;
    xMemPoolInitCallBack InitCB;
    void *Buffer;
    unsigned short Size;
    unsigned short NumRealloc;
    unsigned int Total;
};

typedef void(*xMemBaseNotifyFunc)();

extern xMemInfo_tag gMemInfo;
extern unsigned int gActiveHeap;
extern xMemHeap_tag gxHeap[3];

void xMemInitHeap(xMemHeap_tag *heap, unsigned int base, unsigned int size,
                  unsigned int flags);
void xMemInit();
void *xMemAlloc(unsigned int heapID, unsigned int size, int align);
void *xMemPushTemp(unsigned int size);
void xMemPopTemp(void *addr);
int xMemPushBase(unsigned int heapID);
int xMemPushBase();
int xMemPopBase(unsigned int heapID, int depth);
int xMemPopBase(int depth);
int xMemGetBase(unsigned int heapID);
void xMemRegisterBaseNotifyFunc(xMemBaseNotifyFunc func);
int xMemGetBase();

#endif