#ifndef XMEMMGR_H
#define XMEMMGR_H

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

#endif