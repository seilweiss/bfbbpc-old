#include "zAssetTypes.h"

#include "xstransvc.h"
#include "xpkrsvc.h"
#include "xAnim.h"
#include "xString.h"
#include "xJSP.h"

#include "print.h"

#include <rwcore.h>

typedef xAnimTable *(*AnimTableConstructor)();

struct AnimTableList
{
    const char *name;
    AnimTableConstructor constructor;
    unsigned int id;
};

static void *Curve_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                        unsigned int *outsize);
static void *SndInfoRead(void *, unsigned int assetid, void *indata, unsigned int insize,
                         unsigned int *outsize);
static void MovePoint_Unload(void *userdata, unsigned int);
static void *ATBL_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                       unsigned int *outsize);
static void LightKit_Unload(void *userdata, unsigned int);
static void TextureRW3_Unload(void *userdata, unsigned int);
static void *RWTX_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                       unsigned int *outsize);
static void Anim_Unload(void *userdata, unsigned int);
static void Model_Unload(void *userdata, unsigned int);
static void *Model_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                        unsigned int *outsize);
static void JSP_Unload(void *userdata, unsigned int);
static void *JSP_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                      unsigned int *outsize);
static void BSP_Unload(void *userdata, unsigned int);
static void *BSP_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                      unsigned int *outsize);

static st_PACKER_ASSETTYPE assetTypeHandlers[] =
{
    'BSP ', 0, 0, BSP_Read, NULL, NULL, NULL, NULL, BSP_Unload, NULL,
    'JSP ', 0, 0, JSP_Read, NULL, NULL, NULL, NULL, JSP_Unload, NULL,
    'TXD ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'MODL', 0, 0, Model_Read, NULL, NULL, NULL, NULL, Model_Unload, NULL,
    'ANIM', 0, 0, NULL, NULL, NULL, NULL, NULL, Anim_Unload, NULL,
    'RWTX', 0, 0, RWTX_Read, NULL, NULL, NULL, NULL, TextureRW3_Unload, NULL,
    'LKIT', 0, 0, NULL, NULL, NULL, NULL, NULL, LightKit_Unload, NULL,
    'CAM ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PLYR', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'NPC ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'ITEM', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PKUP', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'TRIG', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SDF ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'TEX ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'TXT ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'ENV ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'ATBL', 0, 0, ATBL_Read, NULL, NULL, NULL, NULL, NULL, NULL,
    'MINF', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PICK', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PLAT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PEND', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'MRKR', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'MVPT', 0, 0, NULL, NULL, NULL, NULL, NULL, MovePoint_Unload, NULL,
    'TIMR', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'CNTR', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PORT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SND ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SNDS', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'GRUP', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'MPHT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SFX ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SNDI', 0, 0, SndInfoRead, NULL, NULL, NULL, NULL, NULL, NULL,
    'HANG', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SIMP', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'BUTN', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SURF', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'DSTR', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'BOUL', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'MAPR', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'GUST', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'VOLU', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'UI  ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'UIFT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'TEXT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'COND', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'DPAT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PRJT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'LOBM', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'FOG ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'LITE', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PARE', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PARS', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'CSN ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'CTOC', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'CSNM', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'EGEN', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'ALST', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'RAW ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'LODT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SHDW', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'DYNA', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'VIL ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'VILP', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'COLL', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PARP', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'PIPT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'DSCO', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'JAW ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SHRP', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'FLY ', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'TRCK', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'CRV ', 0, 0, Curve_Read, NULL, NULL, NULL, NULL, NULL, NULL,
    'ZLIN', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'DUPC', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'SLID', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    'CRDT', 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    0
};

static xJSPHeader *sTempJSP;
static xJSPHeader sDummyEmptyJSP;

static void jsp_shadow_hack(xJSPHeader *jsp);

#include "zNPCTypeTest.h"
#include "zNPCTypeDutchman.h"
#include "zNPCTypeDuplotron.h"
#include "zNPCTypeCommon.h"
#include "zNPCTypeBossPlankton.h"
#include "zNPCTypeBossSandy.h"
#include "zNPCTypeRobot.h"
#include "zNPCTypeKingJelly.h"
#include "zNPCTypeVillager.h"
#include "zNPCTypePrawn.h"
#include "zNPCTypeAmbient.h"
#include "zNPCTypeBossSB1.h"
#include "zNPCTypeBoss.h"
#include "zNPCTypeBossSB2.h"
#include "zNPCTypeTiki.h"
#include "zNPCTypeBossPatrick.h"

AnimTableList animTable[] =
{
    "ZNPC_AnimTable_Test", ZNPC_AnimTable_Test, 0,
    "ZNPC_AnimTable_Dutchman", ZNPC_AnimTable_Dutchman, 0,
    "ZNPC_AnimTable_Duplotron", ZNPC_AnimTable_Duplotron, 0,
    "ZNPC_AnimTable_Common", ZNPC_AnimTable_Common, 0,
    "ZNPC_AnimTable_BossPlankton", ZNPC_AnimTable_BossPlankton, 0,
    "ZNPC_AnimTable_BossSandy", ZNPC_AnimTable_BossSandy, 0,
    "ZNPC_AnimTable_SleepyTime", ZNPC_AnimTable_SleepyTime, 0,
    "ZNPC_AnimTable_BossSandyHead", ZNPC_AnimTable_BossSandyHead, 0,
    "ZNPC_AnimTable_Hammer", ZNPC_AnimTable_Hammer, 0,
    "ZNPC_AnimTable_TTSauce", ZNPC_AnimTable_TTSauce, 0,
    "ZNPC_AnimTable_KingJelly", ZNPC_AnimTable_KingJelly, 0,
    "ZNPC_AnimTable_Slick", ZNPC_AnimTable_Slick, 0,
    "ZNPC_AnimTable_TarTar", ZNPC_AnimTable_TarTar, 0,
    "ZNPC_AnimTable_Villager", ZNPC_AnimTable_Villager, 0,
    "ZNPC_AnimTable_BalloonBoy", ZNPC_AnimTable_BalloonBoy, 0,
    "ZNPC_AnimTable_Fodder", ZNPC_AnimTable_Fodder, 0,
    "ZNPC_AnimTable_Prawn", ZNPC_AnimTable_Prawn, 0,
    "ZNPC_AnimTable_Neptune", ZNPC_AnimTable_Neptune, 0,
    "ZNPC_AnimTable_BossSB1", ZNPC_AnimTable_BossSB1, 0,
    "ZNPC_AnimTable_BossSBobbyArm", ZNPC_AnimTable_BossSBobbyArm, 0,
    "ZNPC_AnimTable_Monsoon", ZNPC_AnimTable_Monsoon, 0,
    "ZNPC_AnimTable_ArfDog", ZNPC_AnimTable_ArfDog, 0,
    "ZNPC_AnimTable_ArfArf", ZNPC_AnimTable_ArfArf, 0,
    "ZNPC_AnimTable_BossSB2", ZNPC_AnimTable_BossSB2, 0,
    "ZNPC_AnimTable_Tiki", ZNPC_AnimTable_Tiki, 0,
    "ZNPC_AnimTable_Tubelet", ZNPC_AnimTable_Tubelet, 0,
    "ZNPC_AnimTable_Ambient", ZNPC_AnimTable_Ambient, 0,
    "ZNPC_AnimTable_GLove", ZNPC_AnimTable_GLove, 0,
    "ZNPC_AnimTable_LassoGuide", ZNPC_AnimTable_LassoGuide, 0,
    "ZNPC_AnimTable_Chuck", ZNPC_AnimTable_Chuck, 0,
    "ZNPC_AnimTable_Jelly", ZNPC_AnimTable_Jelly, 0,
    "ZNPC_AnimTable_SuperFriend", ZNPC_AnimTable_SuperFriend, 0,
    "ZNPC_AnimTable_BossPatrick", ZNPC_AnimTable_BossPatrick, 0
};

static void ATBL_Init();

static RwTexture *TexCB(RwTexture *texture, void *data);

void zAssetStartup()
{
    xSTStartup(assetTypeHandlers);
    ATBL_Init();
}

void zAssetShutdown()
{
    BFBBSTUB("zAssetShutdown");
}

static void *Model_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                        unsigned int *outsize)
{
    BFBBSTUB("Model_Read");
    return NULL;
}

static void *Curve_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                        unsigned int *outsize)
{
    BFBBSTUB("Curve_Read");
    return NULL;
}

static void Model_Unload(void *userdata, unsigned int)
{
    BFBBSTUB("Model_Unload");
}

static void *BSP_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                      unsigned int *outsize)
{
    BFBBSTUB("BSP_Read");
    return NULL;
}

static void BSP_Unload(void *userdata, unsigned int)
{
    BFBBSTUB("BSP_Unload");
}

static void jsp_shadow_hack(xJSPHeader *jsp)
{
    BFBBSTUB("jsp_shadow_hack");
}

static void *JSP_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                      unsigned int *outsize)
{
    xJSPHeader *retjsp;

    retjsp = &sDummyEmptyJSP;
    *outsize = sizeof(xJSPHeader);

    xJSP_MultiStreamRead(indata, insize, &sTempJSP);

    if (sTempJSP->jspNodeList)
    {
        retjsp = sTempJSP;
        sTempJSP = NULL;
        *outsize = sizeof(xJSPHeader *);
    }

    jsp_shadow_hack(retjsp);

    return retjsp;
}

static void JSP_Unload(void *userdata, unsigned int)
{
    if (userdata != &sDummyEmptyJSP)
    {
        xJSP_Destroy((xJSPHeader *)userdata);
    }
}

static RwTexture *TexCB(RwTexture *texture, void *data)
{
    RwTexture **texFound = (RwTexture **)data;

    if (!*texFound)
    {
        *texFound = texture;
    }

    return texture;
}

static void *RWTX_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                       unsigned int *outsize)
{
    RwTexDictionary *txd;
    RwMemory rwmem;
    RwStream *stream;
    RwTexture *tex = NULL;
    RwError error;

    if (insize)
    {
        rwmem.start = (RwUInt8 *)indata;
        rwmem.length = insize;

        stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &rwmem);

        if (stream)
        {
            if (!RwStreamFindChunk(stream, rwID_TEXDICTIONARY, NULL, NULL))
            {
                RwErrorGet(&error);
                RwStreamFindChunk(stream, rwID_TEXDICTIONARY, NULL, NULL);
                RwStreamClose(stream, NULL);
            }
            else
            {
                txd = RwTexDictionaryStreamRead(stream);

                RwStreamClose(stream, NULL);

                if (txd)
                {
                    RwTexDictionaryForAllTextures(txd, TexCB, &tex);

                    if (!tex)
                    {
                        RwTexDictionaryDestroy(txd);
                    }
                    else
                    {
                        RwTexDictionaryRemoveTexture(tex);
                        RwTexDictionaryDestroy(txd);

                        RwTextureAddRef(tex);
                        RwTextureSetFilterMode(tex, rwFILTERLINEARMIPLINEAR);

                        *outsize = sizeof(RwTexture);
                        return tex;
                    }
                }
            }
        }
    }

    *outsize = insize;
    return NULL;
}

static void TextureRW3_Unload(void *userdata, unsigned int)
{
    RwTexture *tex = (RwTexture *)userdata;

    if (tex)
    {
        tex->refCount = 1;
        RwTextureDestroy(tex);
    }
}

static void ATBL_Init()
{
    for (int i = 0; i < sizeof(animTable) / sizeof(AnimTableList); i++)
    {
        animTable[i].id = xStrHash(animTable[i].name);
    }
}

void FootstepHackSceneEnter()
{
    BFBBSTUB("FootstepHackSceneEnter");
}

static void *ATBL_Read(void *, unsigned int assetid, void *indata, unsigned int insize,
                       unsigned int *outsize)
{
    BFBBSTUB("ATBL_Read");
    return NULL;
}

static void Anim_Unload(void *userdata, unsigned int)
{
    BFBBSTUB("Anim_Unload");
    return;
}

static void LightKit_Unload(void *userdata, unsigned int)
{
    BFBBSTUB("LightKit_Unload");
}

static void MovePoint_Unload(void *userdata, unsigned int)
{
    BFBBSTUB("MovePoint_Unload");
}

static void *SndInfoRead(void *, unsigned int assetid, void *indata, unsigned int insize,
                         unsigned int *outsize)
{
    BFBBSTUB("SndInfoRead");
    return NULL;
}