#ifndef IANIM_H
#define IANIM_H

extern unsigned char *giAnimScratch;

void iAnimInit();
float iAnimDuration(void *RawData);
unsigned int iAnimBoneCount(void *RawData);

#endif