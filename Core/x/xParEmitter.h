#ifndef XPAREMITTER_H
#define XPAREMITTER_H

#include "xBase.h"
#include "xMath3.h"
#include "xModel.h"
#include "xParGroup.h"
#include "xParSys.h"

struct xPECircle
{
	float radius;
	float deflection;
	xVec3 dir;
};

struct _tagEmitSphere
{
	float radius;
};

struct _tagEmitRect
{
	float x_len;
	float z_len;
};

struct _tagEmitLine
{
	xVec3 pos1;
	xVec3 pos2;
	float radius;
};

struct _tagEmitVolume
{
	unsigned int emit_volumeID;
};

struct _tagEmitOffsetPoint
{
	xVec3 offset;
};

struct xPEVCyl
{
	float height;
	float radius;
	float deflection;
};

struct xPEEntBone
{
	unsigned char flags;
	unsigned char type;
	unsigned char bone;
	unsigned char pad1;
	xVec3 offset;
	float radius;
	float deflection;
};

struct xPEEntBound
{
	unsigned char flags;
	unsigned char type;
	unsigned char pad1;
	unsigned char pad2;
	float expand;
	float deflection;
};

struct xParInterp
{
	float val[2];
	unsigned int interp;
	float freq;
	float oofreq;
};

struct xParEmitterPropsAsset : xBaseAsset
{
	unsigned int parSysID;
	union
	{
		xParInterp rate;
		xParInterp value[1];
	};
	xParInterp life;
	xParInterp size_birth;
	xParInterp size_death;
	xParInterp color_birth[4];
	xParInterp color_death[4];
	xParInterp vel_scale;
	xParInterp vel_angle;
	xVec3 vel;
	unsigned int emit_limit;
	float emit_limit_reset_time;
};

struct xParEmitterCustomSettings : xParEmitterPropsAsset
{
	unsigned int custom_flags;
	unsigned int attachToID;
	xVec3 pos;
	xVec3 vel;
	float vel_angle_variation;
	unsigned char rot[3];
	unsigned char padding;
	float radius;
	float emit_interval_current;
	void *emit_volume;
};

struct xParEmitterAsset : xBaseAsset
{
	unsigned char emit_flags;
	unsigned char emit_type;
	unsigned short pad;
	unsigned int propID;
	union
	{
		xPECircle e_circle;
		_tagEmitSphere e_sphere;
		_tagEmitRect e_rect;
		_tagEmitLine e_line;
		_tagEmitVolume e_volume;
		_tagEmitOffsetPoint e_offsetp;
		xPEVCyl e_vcyl;
		xPEEntBone e_entbone;
		xPEEntBound e_entbound;
	};
	unsigned int attachToID;
	xVec3 pos;
	xVec3 vel;
	float vel_angle_variation;
	unsigned int cull_mode;
	float cull_dist_sqr;
};

struct xParEmitter : xBase
{
	xParEmitterAsset *tasset;
	xParGroup *group;
	xParEmitterPropsAsset *prop;
	unsigned char rate_mode;
	float rate;
	float rate_time;
	float rate_fraction;
	float rate_fraction_cull;
	unsigned char emit_flags;
	unsigned char emit_pad[3];
	unsigned char rot[3];
	xModelTag tag;
	float oocull_distance_sqr;
	float distance_to_cull_sqr;
	void *attachTo;
	xParSys *parSys;
	void *emit_volume;
	xVec3 last_attach_loc;
};

void xParEmitterInit(void *b, void *tasset);

#endif