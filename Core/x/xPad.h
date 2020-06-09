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
    unsigned int on; // bitmask of held buttons
    unsigned int pressed; // bitmask of just pressed buttons
    unsigned int released; // bitmask of just released buttons
    _tagPadAnalog analog1; // position of left analog stick
    _tagPadAnalog analog2; // position of right analog stick
    _tagPadState state;
    unsigned int flags;
    _tagxRumble rumble_head;
    short port;
    short slot;
    _tagiPad context;
    float al2d_timer; // left analog stick timer
    float ar2d_timer; // right analog stick timer
    float d_timer; // dpad timer
    float up_tmr[22]; // up timer for all buttons
    float down_tmr[22]; // down timer for all buttons
    analog_data analog[2];
};

#ifdef GAMECUBE
#define XPAD_BUTTON_START    0x1
#define XPAD_BUTTON_UP       0x10
#define XPAD_BUTTON_RIGHT    0x20
#define XPAD_BUTTON_DOWN     0x40
#define XPAD_BUTTON_LEFT     0x80
#define XPAD_BUTTON_L1       0x100
#define XPAD_BUTTON_R1       0x1000
#define XPAD_BUTTON_X        0x10000
#define XPAD_BUTTON_CIRCLE   0x20000
#define XPAD_BUTTON_TRIANGLE 0x40000
#define XPAD_BUTTON_SQUARE   0x80000
#define XPAD_BUTTON_R2       0x100000
#define XPAD_BUTTON_ALL      0xFFFFFFFF

#define XPAD_BUTTON_L2       0 // unknown
#define XPAD_BUTTON_SELECT   0 // unknown
#define XPAD_BUTTON_L_ANALOG 0 // unknown
#define XPAD_BUTTON_R_ANALOG XPAD_BUTTON_R2
#elif _WIN32
#define XPAD_BUTTON_START    0x1
#define XPAD_BUTTON_UP       0x10
#define XPAD_BUTTON_RIGHT    0x20
#define XPAD_BUTTON_DOWN     0x40
#define XPAD_BUTTON_LEFT     0x80
#define XPAD_BUTTON_L1       0x100
#define XPAD_BUTTON_R1       0x1000
#define XPAD_BUTTON_X        0x10000
#define XPAD_BUTTON_CIRCLE   0x20000
#define XPAD_BUTTON_TRIANGLE 0x40000
#define XPAD_BUTTON_SQUARE   0x80000
#define XPAD_BUTTON_R2       0x100000

#define XPAD_BUTTON_DBG_CAM_TOGGLE 0x10000000
#define XPAD_BUTTON_DBG_CAM_UP     0x20000000
#define XPAD_BUTTON_DBG_CAM_DOWN   0x40000000
#define XPAD_BUTTON_DBG_CAM_SPRINT 0x80000000

#define XPAD_BUTTON_ALL      0xFFFFFFFF

#define XPAD_BUTTON_L2       0 // unknown
#define XPAD_BUTTON_SELECT   0 // unknown
#define XPAD_BUTTON_L_ANALOG 0 // unknown
#define XPAD_BUTTON_R_ANALOG 0 // unknown
#endif

#define XPAD_ANALOG_REPEAT_DELAY 0.35f

extern _tagxPad mPad[4];
extern _tagxPad *gDebugPad;
extern _tagxPad *gPlayerPad;

int xPadInit();
void xPadRumbleEnable(int idx, int enable);
_tagxPad *xPadEnable(int idx);
void xPadAnalogIsDigital(int idx, int enable);
int xPadUpdate(int idx, float time_passed);
void xPadKill();

#endif