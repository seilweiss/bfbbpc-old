#ifndef ZPLATFORM_H
#define ZPLATFORM_H

#include "zEnt.h"
#include "xEntMotion.h"
#include "xEntDrive.h"

#define XPLATFORM_TYPE_PLATFORM    0
#define XPLATFORM_TYPE_ER          0
#define XPLATFORM_TYPE_ORBIT       1
#define XPLATFORM_TYPE_SPLINE      2
#define XPLATFORM_TYPE_MP          3
#define XPLATFORM_TYPE_MECH        4
#define XPLATFORM_TYPE_PEN         5
#define XPLATFORM_TYPE_CONVBELT    6
#define XPLATFORM_TYPE_FALLING     7
#define XPLATFORM_TYPE_FR          8
#define XPLATFORM_TYPE_BREAKAWAY   9
#define XPLATFORM_TYPE_SPRINGBOARD 10
#define XPLATFORM_TYPE_TEETER      11
#define XPLATFORM_TYPE_PADDLE      12
#define XPLATFORM_TYPE_FM          13

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

struct xScene;

void zPlatform_Init(void *plat, void *asset);
void zPlatform_Setup(zPlatform *plat, xScene *sc);
void zPlatform_Reset(zPlatform *plat, xScene *sc);

#endif