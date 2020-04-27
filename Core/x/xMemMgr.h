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
void xMemRegisterBaseNotifyFunc(xMemBaseNotifyFunc func);

#endif