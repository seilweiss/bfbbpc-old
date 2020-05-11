#ifndef ZCAMMARKER_H
#define ZCAMMARKER_H

#include "xBase.h"
#include "xMath3.h"

enum _tagTransType
{
    eTransType_None,
    eTransType_Interp1,
    eTransType_Interp2,
    eTransType_Interp3,
    eTransType_Interp4,
    eTransType_Linear,
    eTransType_Interp1Rev,
    eTransType_Interp2Rev,
    eTransType_Interp3Rev,
    eTransType_Interp4Rev,
    eTransType_Total
};

struct _tagxCamFollowAsset
{
    float rotation;
    float distance;
    float height;
    float rubber_band;
    float start_speed;
    float end_speed;
};

struct _tagxCamShoulderAsset
{
    float distance;
    float height;
    float realign_speed;
    float realign_delay;
};

struct _tagp2CamStaticAsset
{
    unsigned int unused;
};

struct _tagxCamPathAsset
{
    unsigned int assetID;
    float time_end;
    float time_delay;
};

struct _tagp2CamStaticFollowAsset
{
    float rubber_band;
};

struct xCamAsset : xBaseAsset
{
    xVec3 pos;
    xVec3 at;
    xVec3 up;
    xVec3 right;
    xVec3 view_offset;
    short offset_start_frames;
    short offset_end_frames;
    float fov;
    float trans_time;
    _tagTransType trans_type;
    unsigned int flags;
    float fade_up;
    float fade_down;
    union
    {
        _tagxCamFollowAsset cam_follow;
        _tagxCamShoulderAsset cam_shoulder;
        _tagp2CamStaticAsset cam_static;
        _tagxCamPathAsset cam_path;
        _tagp2CamStaticFollowAsset cam_staticFollow;
    };
    unsigned int valid_flags;
    unsigned int markerid[2];
    unsigned char cam_type;
    unsigned char pad[3];
};

struct zCamMarker : xBase
{
    xCamAsset *asset;
};

void zCamMarkerInit(xBase *b, xCamAsset *asset);

#endif