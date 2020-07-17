#include "xAnim.h"

#include "iAnim.h"
#include "xString.h"
#include "xMorph.h"

#include "print.h"

static xMemPool sxAnimTempTranPool;
unsigned int gxAnimUseGrowAlloc;

static bool _xSingleCompare(char c1, char c2)
{
    switch (c2)
    {
    case '?':
    case '*':
    case '+':
    {
        return true;
    }
    case '#':
    {
        return (c1 >= '0' && c1 <= '9');
    }
    }

    return (c1 == c2);
}

static bool _xSingleCompare(char c, const char *s)
{
    while (*s)
    {
        if (_xSingleCompare(c, *s))
        {
            return true;
        }

        s++;
    }

    return false;
}

static bool _xCharIn(char c, const char *s)
{
    while (*s)
    {
        if (c == *s)
        {
            return true;
        }
        s++;
    }

    return false;
}

#if 0
static bool _xCheckAnimNameInner(const char *name, const char *pattern,
                                 int patternSize, char *extra, int *nameOut,
                                 int *extraOut)
{
    // r25 = extra
    // r22 = name
    // r23 = pattern
    // r24 = patternSize
    // r26 = nameOut
    // r27 = extraOut
    // r30 = r25

    char *r30 = extra;

    const char *r31 = NULL;
    int r29 = 0;
    int r28 = 0;

    while (r29 < patternSize)
    {
        int r3 = pattern[r29];

        switch (r3)
        {
        case '+':
        case '?':
        {
            // loc_80005A90
            r29++;

            if (r28 == 0)
            {
                return false;
            }

            r28++;

            if (r3 != '?')
            {
        case '*':
                // loc_80005AB0
                int r20 = r29 + 1;
                char nextPattern[128] = { 0 };

                *nextPattern = pattern[r20];

                while (*nextPattern == '{' ||
                       *nextPattern == '}' ||
                       *nextPattern == '<' ||
                       *nextPattern == '>')
                {
                    *nextPattern = pattern[++r20];
                }

                if (*nextPattern == '(')
                {
                    const char *r16 = pattern + r20;
                    int r21 = 0;
                    bool r19 = true;
                    int r18 = 0;

                    while (true)
                    {
                        if (*r16 == '\0' || r18 < 0)
                        {
                            nextPattern[r21] = '\0';
                            break;
                        }

                        if (*r16 == '(')
                        {
                            r18++;
                        }
                        else if (*r16 == ')')
                        {
                            r18--;
                        }
                        else if (r18 == 0)
                        {
                            const char *IGNORE_PATTERNS = "{}()<>";

                            if (*r16 == '|')
                            {
                                r19 = true;
                            }
                            else if (r19 && !_xCharIn(*r16, IGNORE_PATTERNS))
                            {
                                r19 = false;
                                nextPattern[r21++] = *r16;
                            }
                        }

                        r20++;
                        r16++;
                    }
                }

                const char *r16 = name + r28;

                while (*r16 && !_xSingleCompare(*r16, nextPattern))
                {
                    r28++;
                    r16++;
                }

                r29++;
            }

            break;
        }
        case '#':
        {
            // loc_80005C00
            if (name[r28] < '0' || name[r28] > '9')
            {
                return false;
            }

            r28++;

            const char *r3 = name + r28;

            while (*r3 >= '0' && *r3 <= '9')
            {
                r28++;
                r3++;
            }

            r29++;

            break;
        }
        case '{':
        {
            // loc_80005C4C
            r31 = name + r28;
            r29++;

            break;
        }
        case '}':
        {
            // loc_80005C58
            int r16 = (size_t)name + r28 - (size_t)r31;

            if (extra)
            {
                memcpy(extra, r31, r16);

                extra[r16] = '\0';
                extra[r16 + 1] = '\1';

                extra += r16 + 1;
            }

            r31 = NULL;
            r29++;

            break;
        }
        case '(':
        {
            // loc_80005C98
            r29++;

            bool r18 = false;
            const char *r21 = pattern + r29;
            const char *r19 = r21;

            while (*r19 != ')' && *r19)
            {
                const char *r4 = r19;

                while (*r19 && *r19 != ')' && *r19 != '|')
                {
                    if (*r19 == '(')
                    {
                        int r3 = 1;

                        while (*r19 && r3 > 0)
                        {
                            if (*r19 == ')')
                            {
                                r3--;
                            }
                            else if (*r19 == '(')
                            {
                                r3++;
                            }

                            r19++;
                        }

                        if (*r19)
                        {
                            r19++;
                        }
                    }
                    else
                    {
                        r19++;
                    }
                }

                if (r19 != r4)
                {
                    bool r16 = false;

                    int var_CC;
                    int var_D0;

                    if (!r18 && _xCheckAnimNameInner(name + r28, r4, r19 - r4, extra, &var_CC, &var_D0))
                    {
                        r16 = true;
                    }

                    if (r16)
                    {
                        r18 = true;
                        r28 += var_CC;
                        extra += var_D0;
                    }
                    else if (extra)
                    {
                        *extra = '\1';
                    }
                }

                if (*r19 == '|')
                {
                    r19++;
                }
            }

            if (*r19)
            {
                r19++;
            }

            r29 += r19 - r21;

            if (!r18)
            {
                return false;
            }

            break;
        }
        case '<':
        {
            // loc_80005DD8
            r29++;

            const char *r20 = pattern + r29;
            const char *r18 = r20;
            const char *r4 = r20;

            while (*r18 && *r18 != ';' && *r18 != '>')
            {
                r18++;
            }

            const char *r5 = r18;

            const char *r19 = NULL;
            const char *r17 = NULL;

            if (*r18 == ';')
            {
                r18++;
                r19 = r18;

                while (*r18 && *r18 != '>')
                {
                    r18++;
                }

                r17 = r18;
            }

            const char *r21 = name + r28;

            int var_D4;
            int var_D8;

            bool r16 = _xCheckAnimNameInner(r21, r4, r5 - r4, extra, &var_D4, &var_D8);

            if (r16)
            {
                if (r19 && _xCheckAnimNameInner(r21, r19, r17 - r19, NULL, NULL, NULL))
                {
                    if (extra)
                    {
                        *extra = '\1';
                    }

                    r16 = false;
                }
                else
                {
                    r28 += var_D4;
                    extra += var_D8;
                }
            }

            if (*r18)
            {
                r18++;
            }

            r29 += r18 - r20;

            if (!r16)
            {
                return false;
            }

            break;
        }
        case '\0':
        {
            // loc_80005EEC
            return false;
        }
        case ')':
        default:
        {
            // loc_80005EF4
            if (name[r28] != r3)
            {
                return false;
            }

            r28++;
            r29++;
        }
        }
    }

    if (nameOut)
    {
        *nameOut = r28;
    }

    if (extraOut)
    {
        *extraOut = extra - r30;
    }

    return true;
}
#else
static bool _xCheckAnimNameInner(const char *name, const char *pattern,
                                 int patternSize, char *extra, int *nameOut,
                                 int *extraOut)
{
    BFBBSTUB("_xCheckAnimNameInner");
    return false;
}
#endif

static bool _xCheckAnimName(const char *name, const char *pattern, char *extra)
{
    *extra = '\1';

    int var_18;

    return (_xCheckAnimNameInner(name, pattern, strlen(pattern), extra, &var_18, NULL) &&
            name[var_18] == '\0');
}

void xAnimInit()
{
    iAnimInit();
    memset(&sxAnimTempTranPool, 0, sizeof(xMemPool));
}

void xAnimTempTransitionInit(unsigned int count)
{
    xMemPoolSetup(&sxAnimTempTranPool, xMemAlloc(count * sizeof(xAnimTransition), 0),
                  0, 0, NULL, sizeof(xAnimTransition), count, count / 2);
}

static float CalcRecipBlendMax(unsigned short *r3)
{
    float f4 = 0.0f;

    while (r3[0] != 0xFFFF)
    {
        float f3;

        if (r3[1] == 0)
        {
            f3 = 0.0f;
        }
        else
        {
            f3 = 1.0f / (r3[1] / 1024.0f);
        }

        float f0 = r3[0] / 1024.0f + f3;

        if (f0 > f4)
        {
            f4 = f0;
        }

        r3 += 2;
    }

    return (0.0f == f4) ? 0.0f : (1.0f / f4);
}

static int StateHasTransition(xAnimState *state, xAnimTransition *tran)
{
    xAnimTransitionList *list = state->List;

    while (list)
    {
        if (list->T == tran)
        {
            return 1;
        }

        list = list->Next;
    }

    return 0;
}

static int DefaultHasTransition(xAnimState *state, xAnimTransition *tran, unsigned int *r5)
{
    xAnimTransitionList *list = state->Default;
    unsigned int r7 = 1;

    while (list)
    {
        if (list->T == tran)
        {
            return 1;
        }

        if (list->T->Conditional == tran->Conditional &&
            list->T->UserFlags == tran->UserFlags)
        {
            r7 = 0;
        }

        list = list->Next;
    }

    *r5 += r7;
    return 0;
}

static int DefaultOverride(xAnimState *state, xAnimTransition *tran)
{
    xAnimTransitionList *list = state->Default;

    while (list)
    {
        if (list->T->Conditional == tran->Conditional &&
            list->T->UserFlags == tran->UserFlags)
        {
            list->T = tran;
            return 1;
        }

        list = list->Next;
    }

    return 0;
}

static void TransitionTimeInit(xAnimSingle *single, xAnimTransition *tran)
{
    if (tran->Flags & XANIMTRANSITION_UNK20)
    {
        if ((single->State->Data->FileFlags ^ tran->Dest->Data->FileFlags) & XANIMFILE_UNK1000)
        {
            single->Time = tran->Dest->Data->Duration - single->Time;
        }
    }
    else
    {
        single->Time =
            ((tran->Dest->Flags & XANIMSTATE_UNK100) && (0.0f == tran->DestTime)) ?
            (single->State->Data->Duration * xurand()) :
            tran->DestTime;
    }

    single->LastTime = single->Time;
}

xAnimFile *xAnimFileNewBilinear(void **rawData, const char *name, unsigned int flags,
                                xAnimFile **linkedList, unsigned int numX,
                                unsigned int numY)
{
    int i;
    xAnimFile *afile;

    afile = gxAnimUseGrowAlloc ?
        (xAnimFile *)xMemGrowAlloc(numX * numY * sizeof(void *) + sizeof(xAnimFile)) :
        (xAnimFile *)xMemAlloc(numX * numY * sizeof(void *) + sizeof(xAnimFile), 0);

    if (numX > 1 || numY > 1)
    {
        flags |= XANIMFILE_UNK4000;
    }

    if (*(unsigned int *)*rawData == 'QSPM')
    {
        flags |= XANIMFILE_UNK8000;
    }

    if (linkedList)
    {
        afile->Next = *linkedList;
        *linkedList = afile;
    }
    else
    {
        afile->Next = NULL;
    }

    afile->RawData = (void **)(afile + 1);

    for (i = 0; i < numX * numY; i++)
    {
        afile->RawData[i] = rawData[i];
    }

    afile->Name = name;
    afile->ID = xStrHash(afile->Name);
    afile->FileFlags = flags;

    afile->Duration = (afile->FileFlags & XANIMFILE_UNK2000) ?
        (2.0f * ((afile->FileFlags & XANIMFILE_UNK8000) ?
                xMorphSeqDuration((xMorphSeqFile *)afile->RawData[0]) :
                iAnimDuration(afile->RawData[0]))) :
        ((afile->FileFlags & XANIMFILE_UNK8000) ?
        xMorphSeqDuration((xMorphSeqFile *)afile->RawData[0]) :
        iAnimDuration(afile->RawData[0]));

    afile->TimeOffset = 0.0f;
    afile->BoneCount = (flags & XANIMFILE_UNK8000) ? 0 : iAnimBoneCount(rawData[0]);
    afile->NumAnims[0] = numX;
    afile->NumAnims[1] = numY;

    return afile;
}

xAnimFile *xAnimFileNew(void *rawData, const char *name, unsigned int flags,
                        xAnimFile **linkedList)
{
    return xAnimFileNewBilinear(&rawData, name, flags, linkedList, 1, 1);
}

void xAnimFileSetTime(xAnimFile *data, float duration, float timeOffset)
{
    float rawDuration;

    rawDuration = (data->FileFlags & XANIMFILE_UNK8000) ?
        xMorphSeqDuration((xMorphSeqFile *)data->RawData[0]) :
        iAnimDuration(data->RawData[0]);

    if (timeOffset > rawDuration - 0.1f)
    {
        timeOffset = rawDuration - 0.1f;
    }

    data->TimeOffset = timeOffset;

    if (duration > rawDuration - timeOffset)
    {
        duration = rawDuration - timeOffset;
    }

    data->Duration = (data->FileFlags & XANIMFILE_UNK2000) ? 2.0f * duration : duration;
}

#if 0
void xAnimFileEval(xAnimFile *data, float time, float *bilinear, unsigned int flags,
                   xVec3 *tran, xQuat *quat, float *)
{
    // r21 = data
    // r26 = bilinear
    // r22 = flags
    // r23 = tran
    // r24 = quat
    // f31 = time

    // r20 = data->FileFlags

    int i;
    unsigned int numBones; // r25
    float bilerp[2];
    unsigned int biindex[2];
    unsigned int biplus[2];
    xQuat *q0;
    xVec3 *t0;
    xQuat *q1;
    xVec3 *t1;

    time = xAnimFileRawTime(data, xmax(xmin(time, data->Duration), 0.0f));

    if (!(data->FileFlags & XANIMFILE_UNK8000))
    {
        if (flags & 0x1)
        {
            numBones = 1;
        }
        else
        {
            numBones = data->BoneCount;
        }

        if (flags & 0x2)
        {
            numBones--;
        }

        if (numBones)
        {
            if (bilinear && (data->FileFlags & XANIMFILE_UNK4000))
            {
                for (i = 0; i < 2; i++)
                {
                    float f30 = xmax(0.0f, xmin(bilinear[i], data->NumAnims[i] - 1));
                    float f1 = std::floorf(f30);

                    bilerp[i] = f30 - f1;
                    biindex[i] = (unsigned int)f1;

                    biplus[i] =
                        (biindex[i] + 1 < data->NumAnims[i]) ?
                        (biindex[i] + 1) : biindex[i];
                }


            }
            else
            {

            }
        }
    }
}
#else
void xAnimFileEval(xAnimFile *data, float time, float *bilinear, unsigned int flags,
                   xVec3 *tran, xQuat *quat, float *)
{
    BFBBSTUB("xAnimFileEval");
}
#endif

xAnimEffect *xAnimStateNewEffect(xAnimState *state, unsigned int flags, float startTime,
                                 float endTime, xAnimEffectCallBack callback,
                                 unsigned int userDataSize)
{
    BFBBSTUB("xAnimStateNewEffect");
    return NULL;
}

xAnimTable *xAnimTableNew(const char *name, xAnimTable **linkedList,
                          unsigned int userFlags)
{
    BFBBSTUB("xAnimTableNew");
    return NULL;
}

void xAnimDefaultBeforeEnter(xAnimPlay *, xAnimState *state)
{
    BFBBSTUB("xAnimDefaultBeforeEnter");
}

xAnimState *xAnimTableNewState(xAnimTable *table, const char *name, unsigned int flags,
                               unsigned int userFlags, float speed, float *boneBlend,
                               float *timeSnap, float fadeRecip,
                               unsigned short *fadeOffset, void *callbackData,
                               xAnimBeforeEnterCallBack beforeEnter,
                               xAnimStateCallBack stateCallback,
                               xAnimBeforeAnimMatricesCallBack beforeAnimMatrices)
{
    BFBBSTUB("xAnimTableNewState");
    return NULL;
}

static void _xAnimTableAddTransitionHelper(xAnimState *, xAnimTransition *,
                                           unsigned int &, unsigned int &, xAnimState **)
{
    BFBBSTUB("_xAnimTableAddTransitionHelper");
}

static void _xAnimTableAddTransition(xAnimTable *table, xAnimTransition *tran,
                                     const char *source, const char *dest)
{
    BFBBSTUB("_xAnimTableAddTransition");
}

void xAnimTableAddTransition(xAnimTable *table, xAnimTransition *tran,
                             const char *source)
{
    BFBBSTUB("xAnimTableAddTransition");
}

xAnimTransition *xAnimTableNewTransition(xAnimTable *table, const char *source,
                                         const char *dest,
                                         xAnimTransitionCallBack conditional,
                                         xAnimTransitionCallBack callback,
                                         unsigned int flags, unsigned int userFlags,
                                         float srcTime, float destTime,
                                         unsigned short priority,
                                         unsigned short queuePriority, float blendRecip,
                                         unsigned short *blendOffset)
{
    BFBBSTUB("xAnimTableNewTransition");
    return NULL;
}

void xAnimTableAddFile(xAnimTable *table, xAnimFile *file, const char *states)
{
    BFBBSTUB("xAnimTableAddFile");
}

xAnimState *xAnimTableAddFileID(xAnimTable *table, xAnimFile *file, unsigned int stateID,
                                unsigned int subStateID, unsigned int subStateCount)
{
    BFBBSTUB("xAnimTableAddFileID");
    return NULL;
}

xAnimState *xAnimTableGetStateID(xAnimTable *table, unsigned int ID)
{
    BFBBSTUB("xAnimTableGetStateID");
    return NULL;
}

xAnimState *xAnimTableGetState(xAnimTable *table, const char *name)
{
    BFBBSTUB("xAnimTableGetState");
    return NULL;
}

static void EffectActiveInsert(xAnimSingle *, xAnimActiveEffect *)
{
    BFBBSTUB("EffectActiveInsert");
}

static void EffectActiveRemove(xAnimActiveEffect *, unsigned int, unsigned int)
{
    BFBBSTUB("EffectActiveRemove");
}

static int EffectPlaying(xAnimSingle *, xAnimEffect *)
{
    BFBBSTUB("EffectPlaying");
    return 0;
}

static void EffectSingleStart(xAnimSingle *single)
{
    BFBBSTUB("EffectSingleStart");
}

static void EffectSingleDuration(xAnimSingle *single)
{
    BFBBSTUB("EffectSingleDuration");
}

static void EffectSingleRun(xAnimSingle *single)
{
    BFBBSTUB("EffectSingleRun");
}

static void EffectSingleLoop(xAnimSingle *single)
{
    BFBBSTUB("EffectSingleLoop");
}

static void EffectSingleStop(xAnimSingle *single)
{
    BFBBSTUB("EffectSingleStop");
}

static void StopUpdate(xAnimSingle *)
{
    BFBBSTUB("StopUpdate");
}

static void LoopUpdate(xAnimSingle *)
{
    BFBBSTUB("LoopUpdate");
}

void xAnimPlaySetState(xAnimSingle *single, xAnimState *state, float startTime)
{
    BFBBSTUB("xAnimPlaySetState");
}

static void SingleUpdate(xAnimSingle *single, float timeDelta)
{
    BFBBSTUB("SingleUpdate");
}

static void SingleEval(xAnimSingle *, xVec3 *, xQuat *)
{
    BFBBSTUB("SingleEval");
}

void xAnimPlaySetup(xAnimPlay *play, void *object, xAnimTable *table,
                    xModelInstance *modelInst)
{
    BFBBSTUB("xAnimPlaySetup");
}

void xAnimPlayChooseTransition(xAnimPlay *play)
{
    BFBBSTUB("xAnimPlayChooseTransition");
}

void xAnimPlayStartTransition(xAnimPlay *play, xAnimTransition *transition)
{
    BFBBSTUB("xAnimPlayStartTransition");
}

void xAnimPlayUpdate(xAnimPlay *play, float timeDelta)
{
    BFBBSTUB("xAnimPlayUpdate");
}

void xAnimPlayEval(xAnimPlay *play)
{
    BFBBSTUB("xAnimPlayEval");
}

void xAnimPoolCB(xMemPool *pool, void *data)
{
    BFBBSTUB("xAnimPoolCB");
}

void xAnimPoolInit(xMemPool *pool, unsigned int count, unsigned int singles,
                   unsigned int blendFlags, unsigned int effectMax)
{
    BFBBSTUB("xAnimPoolInit");
}

xAnimPlay *xAnimPoolAlloc(xMemPool *pool, void *object, xAnimTable *table,
                          xModelInstance *modelInst)
{
    BFBBSTUB("xAnimPoolAlloc");
    return NULL;
}

void xAnimPoolFree(xAnimPlay *play)
{
    BFBBSTUB("xAnimPoolFree");
}

float xAnimFileRawTime(xAnimFile *data, float f1)
{
    if ((data->FileFlags & XANIMFILE_UNK1000) ||
        ((data->FileFlags & XANIMFILE_UNK2000) &&
         f1 > 0.5f * data->Duration))
    {
        return data->TimeOffset + data->Duration - f1;
    }

    return data->TimeOffset + f1;
}