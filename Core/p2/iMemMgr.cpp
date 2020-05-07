#include "iMemMgr.h"

#include "xMemMgr.h"

#include <windows.h>

static char sStack[0x8000];

static void *mem_top_alloc;
static void *mem_base_alloc;

static HANDLE sHeap;
static size_t sHeapSize;

void iMemInit()
{
    sHeap = GetProcessHeap();

    if (sHeap == NULL)
    {
        exit(-5);
    }

    gMemInfo.system.addr = NULL;
    gMemInfo.system.size = 16;
    gMemInfo.system.flags = 0x20;

    gMemInfo.stack.addr = (size_t)sStack;
    gMemInfo.stack.size = sizeof(sStack);
    gMemInfo.stack.flags = 0x820;

    sHeapSize = 0x384000;

    mem_base_alloc = HeapAlloc(sHeap, 0, sHeapSize);
    mem_top_alloc = (void *)((size_t)mem_base_alloc + sHeapSize);

    gMemInfo.DRAM.addr = (size_t)mem_base_alloc;
    gMemInfo.DRAM.size = sHeapSize;
    gMemInfo.DRAM.flags = 0x820;

    gMemInfo.SRAM.addr = NULL;
    gMemInfo.SRAM.size = 32;
    gMemInfo.SRAM.flags = 0x660;
}

void iMemExit()
{
    //free((void *)gMemInfo.DRAM.addr);
    HeapFree(sHeap, 0, (void *)gMemInfo.DRAM.addr);
    gMemInfo.DRAM.addr = NULL;
}

void null_func()
{
    mem_base_alloc = (void *)((size_t)mem_base_alloc + sizeof(size_t));
}

/*
extern "C"
{
void * __cdecl malloc(size_t size)
{
    void *mem;

    if (size <= 0)
    {
        return NULL;
    }

    mem = HeapAlloc(sHeap, 0, size);

    if (!mem)
    {
        null_func();
    }

    return mem;
}

void __cdecl free(void *block)
{
    if (block)
    {
        HeapFree(sHeap, 0, block);
    }
}
}
*/