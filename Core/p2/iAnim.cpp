#include "iAnim.h"

#include "iAnimSKB.h"

static unsigned char scratchBuffer[9120];
unsigned char *giAnimScratch = scratchBuffer;

void iAnimInit()
{
    return;
}

float iAnimDuration(void *RawData)
{
    return iAnimDurationSKB((iAnimSKBHeader *)RawData);
}

unsigned int iAnimBoneCount(void *RawData)
{
    if (*(unsigned int *)RawData == '1BKS')
    {
        return ((iAnimSKBHeader *)RawData)->BoneCount;
    }

    return 0;
}