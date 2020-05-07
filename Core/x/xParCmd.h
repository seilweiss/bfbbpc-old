#ifndef XPARCMD_H
#define XPARCMD_H

struct xParCmdAsset
{
	unsigned int type;
	unsigned char enabled;
	unsigned char mode;
	unsigned char pad[2];
};

struct xParCmdTex : xParCmdAsset
{
	float x1;
	float y1;
	float x2;
	float y2;
	unsigned char birthMode;
	unsigned char rows;
	unsigned char cols;
	unsigned char unit_count;
	float unit_width;
	float unit_height;
};

struct xParCmd
{
	unsigned int flag;
	xParCmdAsset *tasset;
};

#endif