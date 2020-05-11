#ifndef ZPLATFORM_H
#define ZPLATFORM_H

#include "zEnt.h"
#include "xEntMotion.h"
#include "xEntDrive.h"

struct xPlatformERData
{
    int nodata;
};

struct xPlatformOrbitData
{
    int nodata;
};

struct xPlatformSplineData
{
    int nodata;
};

struct xPlatformMPData
{
    int nodata;
};

struct xPlatformMechData
{
    int nodata;
};

struct xPlatformPenData
{
    int nodata;
};

struct xPlatformConvBeltData
{
    float speed;
};

struct xPlatformFallingData
{
    float speed;
    unsigned int bustModelID;
};

struct xPlatformFRData
{
    float fspeed;
    float rspeed;
    float ret_delay;
    float post_ret_delay;
};

struct xPlatformBreakawayData
{
    float ba_delay;
    unsigned int bustModelID;
    float reset_delay;
    unsigned int breakflags;
};

struct xPlatformSpringboardData
{
    float jmph[3];
    float jmpbounce;
    unsigned int animID[3];
    xVec3 jmpdir;
    unsigned int springflags;
};

struct xPlatformTeeterData
{
    float itilt;
    float maxtilt;
    float invmass;
};

struct xPlatformPaddleData
{
    int startOrient;
    int countOrient;
    float orientLoop;
    float orient[6];
    unsigned int paddleFlags;
    float rotateSpeed;
    float accelTime;
    float decelTime;
    float hubRadius;
};

struct xPlatformFMData
{
    int nothingyet;
};

struct xPlatformAsset
{
    unsigned char type;
    unsigned char pad;
    unsigned short flags;
    union
    {
        xPlatformERData er;
        xPlatformOrbitData orb;
        xPlatformSplineData spl;
        xPlatformMPData mp;
        xPlatformMechData mech;
        xPlatformPenData pen;
        xPlatformConvBeltData cb;
        xPlatformFallingData fall;
        xPlatformFRData fr;
        xPlatformBreakawayData ba;
        xPlatformSpringboardData sb;
        xPlatformTeeterData teet;
        xPlatformPaddleData paddle;
        xPlatformFMData fm;
    };
};

struct zPlatFMRunTime
{
    unsigned int flags;
    float tmrs[12];
    float ttms[12];
    float atms[12];
    float dtms[12];
    float vms[12];
    float dss[12];
};

struct zPlatform : zEnt
{
    xPlatformAsset *passet;
    xEntMotion motion;
    unsigned short state;
    unsigned short plat_flags;
    float tmr;
    int ctr;
    xMovePoint *src;
    xModelInstance *am;
    xModelInstance *bm;
    int moving;
    xEntDrive drv;
    zPlatFMRunTime *fmrt;
    float pauseMult;
    float pauseDelta;
};

void zPlatform_Init(void *plat, void *asset);

#endif