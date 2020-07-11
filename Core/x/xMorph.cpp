#include "xMorph.h"

#include "print.h"

float xMorphSeqDuration(xMorphSeqFile *seq)
{
    return ((float *)(seq + 1))[seq->TimeCount - 1];
}