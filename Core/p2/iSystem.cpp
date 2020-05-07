#include "iSystem.h"

#include "xDebug.h"
#include "xMemMgr.h"
#include "iFile.h"
#include "iTime.h"
#include "xPad.h"
#include "xSnd.h"
#include "xMath.h"
#include "xMath3.h"
#include "xShadow.h"
#include "xFX.h"

#include "print.h"

#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>

#ifndef BFBB_RWSRC
#include <rpskin.h>
#include <rphanim.h>
#include <rpmatfx.h>
#include <rpusrdat.h>
#include <rpptank.h>
#endif

#include <windows.h>

RwVideoMode sVideoMode;
static RwMemoryFunctions MemoryFunctions;

RwMemoryFunctions *psGetMemoryFunctions()
{
    return &MemoryFunctions;
}

// TODO: This is not vsync.
// Fix this lol
void iVSync()
{
    Sleep(16);

    /*
    HWND window = GetActiveWindow();

    MSG msg;
    BOOL bRet;

    while ((bRet = GetMessage(&msg, window, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_PAINT)
        {
            Sleep(16);
            break;
        }
    }
    */
}

static void iTRCInit()
{
    BFBBSTUB("iTRCInit");
}

static unsigned int iRenderWareInit();
static void iRenderWareExit();
static unsigned int RWAttachPlugins();
static RwTexture *TextureRead(const RwChar *name, const RwChar *maskName);
static unsigned int InitMemorySystem(RwMemoryFunctions *memoryFuncs);

void iSystemInit(unsigned int options)
{
    InitMemorySystem(&MemoryFunctions);

    xDebugInit();
    xMemInit();
    iFileInit();
    iTimeInit();
    xPadInit();
    xSndInit();
    iTRCInit();
    iRenderWareInit();
    xMathInit();
    xMath3Init();
}

void iSystemExit()
{
    xDebugExit();
    xMathExit();
    iRenderWareExit();
    xSndExit();
    xPadKill();
    iFileExit();
    iTimeExit();
    xMemExit();

    printf("(With apologies to Jim Morrison) This the end, my only friend, The End.");
}

static unsigned int RWAttachPlugins()
{
    if (!RpWorldPluginAttach())
    {
        return 1;
    }

    if (!RpCollisionPluginAttach())
    {
        return 1;
    }

#ifndef BFBB_RWSRC
    if (!RpSkinPluginAttach())
    {
        return 1;
    }

    if (!RpHAnimPluginAttach())
    {
        return 1;
    }

    if (!RpMatFXPluginAttach())
    {
        return 1;
    }

    if (!RpUserDataPluginAttach())
    {
        return 1;
    }

    if (!RpPTankPluginAttach())
    {
        return 1;
    }
#endif

    return 0;
}

// RenderWare's memory functions are broken, so here are some dumb wrappers :)

static void *bfbbmalloc(size_t size, RwUInt32 hint)
{
    return malloc(size);
}

static void bfbbfree(void *mem)
{
    free(mem);
}

static void *bfbbrealloc(void *mem, size_t newSize, RwUInt32 hint)
{
    return realloc(mem, newSize);
}

static void *bfbbcalloc(size_t numObj, size_t sizeObj, RwUInt32 hint)
{
    return calloc(numObj, sizeObj);
}

static unsigned int InitMemorySystem(RwMemoryFunctions *memoryFuncs)
{
    memoryFuncs->rwmalloc = bfbbmalloc;
    memoryFuncs->rwcalloc = bfbbcalloc;
    memoryFuncs->rwrealloc = bfbbrealloc;
    memoryFuncs->rwfree = bfbbfree;

    return 1;
}

static unsigned int iRenderWareInit()
{
    RwEngineOpenParams openParams;

    if (!RwEngineInit(psGetMemoryFunctions(), 0, 0))
    {
        return 1;
    }

    if (RWAttachPlugins() != 0)
    {
        return 1;
    }

    openParams.displayID = GetActiveWindow();

    if (!RwEngineOpen(&openParams))
    {
        RwEngineTerm();
        return 1;
    }

    RwEngineGetVideoModeInfo(&sVideoMode, RwEngineGetCurrentVideoMode());

    if (!RwEngineStart())
    {
        RwEngineClose();
        RwEngineTerm();
        return 1;
    }

    RwTextureSetReadCallBack(TextureRead);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLBACK);

    xShadowInit();
    xFXInit();

    RwTextureSetMipmapping(TRUE);
    RwTextureSetAutoMipmapping(TRUE);

    return 0;
}

static void iRenderWareExit()
{
    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();
}

static RwTexture *TextureRead(const RwChar *name, const RwChar *maskName)
{
    BFBBSTUB("TextureRead");
    return NULL;
}