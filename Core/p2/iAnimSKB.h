#ifndef IANIMSKB_H
#define IANIMSKB_H

struct iAnimSKBHeader
{
	unsigned int Magic;
	unsigned int Flags;
	unsigned short BoneCount;
	unsigned short TimeCount;
	unsigned int KeyCount;
	float Scale[3];
};

struct iAnimSKBKey
{
	unsigned short TimeIndex;
	short Quat[4];
	short Tran[3];
};

float iAnimDurationSKB(iAnimSKBHeader *data);

#endif