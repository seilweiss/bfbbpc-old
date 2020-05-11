#ifndef XPAD_H
#define XPAD_H

#include "xRumble.h"
#include "iPad.h"
#include "xMath2.h"

struct _tagPadAnalog
{
    char x;
    char y;
};

enum _tagPadState
{
    ePad_Disabled,
    ePad_DisabledError,
    ePad_Enabled,
    ePad_Missing,
    ePad_Total
};

struct analog_data
{
    xVec2 offset;
    xVec2 dir;
    float mag;
    float ang;
};

struct _tagxPad
{
    unsigned char value[22];
    unsigned char last_value[22];
    unsigned int on;
    unsigned int pressed;
    unsigned int released;
    _tagPadAnalog analog1;
    _tagPadAnalog analog2;
    _tagPadState state;
    unsigned int flags;
    _tagxRumble rumble_head;
    short port;
    short slot;
    _tagiPad context;
    float al2d_timer;
    float ar2d_timer;
    float d_timer;
    float up_tmr[22];
    float down_tmr[22];
    analog_data analog[2];
};

extern _tagxPad *gDebugPad;

int xPadInit();
void xPadRumbleEnable(int idx, int enable);
_tagxPad *xPadEnable(int idx);
int xPadUpdate(int idx, float time_passed);
void xPadKill();

#endif