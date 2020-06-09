#include "iPad.h"

#include "xPad.h"
#include "xTRC.h"

#if _WIN32
#include <windows.h>
#endif

#include "print.h"

#if _WIN32
static BYTE sPadData[256];
#elif GAMECUBE
static PADStatus sPadData[PAD_MAX_CONTROLLERS];
#endif

int iPadInit()
{
    return 1;
}

_tagxPad *iPadEnable(_tagxPad *pad, short port)
{
    pad->port = port;
    pad->slot = 0;
    pad->state = ePad_Enabled;

    gTrcPad[pad->port].state = TRC_PadInserted;

    pad->flags |= 0x3;
    pad->flags |= 0x4;

    return pad;
}

int iPadUpdate(_tagxPad *pad, unsigned int *on)
{
    if (pad->port == 0)
    {
        BOOL result = GetKeyboardState(sPadData);

        if (!result)
        {
            return 0;
        }
    }

    unsigned int temp_on = 0;

    if (sPadData[VK_UP] & 0x80)
    {
        temp_on |= XPAD_BUTTON_UP;
    }

    if (sPadData[VK_LEFT] & 0x80)
    {
        temp_on |= XPAD_BUTTON_LEFT;
    }

    if (sPadData[VK_DOWN] & 0x80)
    {
        temp_on |= XPAD_BUTTON_DOWN;
    }

    if (sPadData[VK_RIGHT] & 0x80)
    {
        temp_on |= XPAD_BUTTON_RIGHT;
    }

    if (sPadData[VK_TAB] & 0x80)
    {
        temp_on |= XPAD_BUTTON_DBG_CAM_TOGGLE;
    }

    if (sPadData[VK_SPACE] & 0x80)
    {
        temp_on |= XPAD_BUTTON_DBG_CAM_UP;
    }

    if (sPadData[VK_CONTROL] & 0x80)
    {
        temp_on |= XPAD_BUTTON_DBG_CAM_DOWN;
    }

    if (sPadData[VK_SHIFT] & 0x80)
    {
        temp_on |= XPAD_BUTTON_DBG_CAM_SPRINT;
    }

    *on = temp_on;

    pad->analog1.x = 0;
    pad->analog1.y = 0;
    pad->analog2.x = 0;
    pad->analog2.y = 0;

    if (sPadData['W'] & 0x80)
    {
        pad->analog1.y -= 127;
    }

    if (sPadData['A'] & 0x80)
    {
        pad->analog1.x -= 127;
    }

    if (sPadData['S'] & 0x80)
    {
        pad->analog1.y += 127;
    }

    if (sPadData['D'] & 0x80)
    {
        pad->analog1.x += 127;
    }

    if (sPadData[VK_NUMPAD8] & 0x80)
    {
        pad->analog2.y -= 127;
    }

    if (sPadData[VK_NUMPAD4] & 0x80)
    {
        pad->analog2.x -= 127;
    }

    if (sPadData[VK_NUMPAD2] & 0x80)
    {
        pad->analog2.y += 127;
    }

    if (sPadData[VK_NUMPAD6] & 0x80)
    {
        pad->analog2.x += 127;
    }

    return 1;
}

void iPadRumbleFx(_tagxPad *p, _tagxRumble *r, float time_passed)
{
    return;
}

void iPadStopRumble(_tagxPad *pad)
{
    BFBBSTUB("iPadStopRumble");
}

void iPadStartRumble(_tagxPad *pad, _tagxRumble *rumble)
{
    BFBBSTUB("iPadStartRumble");
}