#ifndef ZSURFACE_H
#define ZSURFACE_H

#include "xSurface.h"
#include "xMath3.h"

struct zSurfMatFX
{
	unsigned int flags;
	unsigned int bumpmapID;
	unsigned int envmapID;
	float shininess;
	float bumpiness;
	unsigned int dualmapID;
};

struct zSurfColorFX
{
	unsigned short flags;
	unsigned short mode;
	float speed;
};

struct zSurfTextureAnim
{
	unsigned short pad;
	unsigned short mode;
	unsigned int group;
	float speed;
};

struct zSurfUVFX
{
	int mode;
	float rot;
	float rot_spd;
	xVec3 trans;
	xVec3 trans_spd;
	xVec3 scale;
	xVec3 scale_spd;
	xVec3 min;
	xVec3 max;
	xVec3 minmax_spd;
};

struct zSurfAssetBase : xBaseAsset
{
	unsigned char game_damage_type;
	unsigned char game_sticky;
	unsigned char game_damage_flags;
	unsigned char surf_type;
	unsigned char phys_pad;
	unsigned char sld_start;
	unsigned char sld_stop;
	unsigned char phys_flags;
	float friction;
	zSurfMatFX matfx;
	zSurfColorFX colorfx;
	unsigned int texture_anim_flags;
	zSurfTextureAnim texture_anim[2];
	unsigned int uvfx_flags;
	zSurfUVFX uvfx[2];
	unsigned char on;
	unsigned char surf_pad[3];
	float oob_delay;
	float walljump_scale_xz;
	float walljump_scale_y;
	float damage_timer;
	float damage_bounce;
};

struct zSurfacePropTexAnim
{
	unsigned short mode;
	float speed;
	float frame;
	unsigned int group;
	int group_idx;
	xBase *group_ptr;
};

struct zSurfacePropUVFX
{
	int mode;
	float rot;
	float rot_spd;
	float minmax_timer[2];
	xVec3 trans;
	xVec3 trans_spd;
	xVec3 scale;
	xVec3 scale_spd;
	xVec3 min;
	xVec3 max;
	xVec3 minmax_spd;
};

struct zSurfaceProps
{
	zSurfAssetBase *asset;
	unsigned int texanim_flags;
	zSurfacePropTexAnim texanim[2];
	unsigned int uvfx_flags;
	zSurfacePropUVFX uvfx[2];
};

void zSurfaceInit();
void zSurfaceRegisterMapper(unsigned int assetId);
void zSurfaceExit();
xSurface *zSurfaceGetSurface(unsigned int mat_id);
void zSurfaceSetup(xSurface *s);

#endif