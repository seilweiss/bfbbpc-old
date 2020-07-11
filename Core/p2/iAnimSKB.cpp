#include "iAnimSKB.h"

float iAnimDurationSKB(iAnimSKBHeader *data)
{
    return ((float *)((iAnimSKBKey *)(data + 1) + data->KeyCount))[data->TimeCount - 1];
}