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

#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rpskin.h>
#include <rphanim.h>
#include <rpmatfx.h>
#include <rpusrdat.h>
#include <rpptank.h>

#include <windows.h>

RwVideoMode sVideoMode;

// STUB
static void iTRCInit()
{

}

static unsigned int iRenderWareInit();
static unsigned int RWAttachPlugins();
static RwTexture *TextureRead(const RwChar *name, const RwChar *maskName);

void iSystemInit(unsigned int options)
{
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

    return 0;
}

static unsigned int iRenderWareInit()
{
    RwEngineOpenParams openParams;

    if (!RwEngineInit(NULL, 0, 0))
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

// STUB
static RwTexture *TextureRead(const RwChar *name, const RwChar *maskName)
{
    return NULL;
}