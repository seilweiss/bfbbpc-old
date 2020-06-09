#include "xPad.h"

#include "zScene.h"
#include "zMenu.h"
#include "zGame.h"
#include "zGameExtras.h"

#include "print.h"

#include <stdlib.h>
#include <string.h>

static _tagxRumble mRumbleList[32];

_tagxPad mPad[4];
_tagxPad *gDebugPad;
_tagxPad *gPlayerPad;

int xPadInit()
{
    memset(mPad, 0, sizeof(mPad));
    memset(mRumbleList, 0, sizeof(mRumbleList));

    if (!iPadInit())
    {
        return 0;
    }

    gPlayerPad = &mPad[0];

    return 1;
}

_tagxPad *xPadEnable(int idx)
{
    _tagxPad *p = &mPad[idx];

    if (p->state != ePad_Disabled)
    {
        return p;
    }

    if (idx != 0)
    {
        return p;
    }

    p = iPadEnable(p, idx);

    xPadRumbleEnable(idx, 1);

    return p;
}

void xPadRumbleEnable(int idx, int enable)
{
    BFBBSTUB("xPadRumbleEnable");
}

int xPadUpdate(int idx, float time_passed)
{
    unsigned int new_on;

    if (idx != 0)
    {
        return 0;
    }

    if (zScene_ScreenAdjustMode())
    {
        xPadAnalogIsDigital(idx, 0);
    }
    else if (zMenuRunning() || zGameIsPaused() || zGame_HackIsGallery())
    {
        xPadAnalogIsDigital(idx, 1);
    }
    else
    {
        xPadAnalogIsDigital(idx, 0);
    }

    _tagxPad *p = &mPad[idx];

    if ((p->flags & 0x4) && (p->flags & 0x8))
    {
        _tagxRumble *r = p->rumble_head.next;

        while (r)
        {
            r->seconds -= time_passed;

            if (r->seconds <= 0.0f)
            {
                r->active = 0;

                if (!r->next)
                {
                    p->rumble_head.next = NULL;
                    r = NULL;

                    iPadStopRumble(p);
                }
                else
                {
                    p->rumble_head.next = r->next;
                    r = p->rumble_head.next;

                    iPadStartRumble(p, r);
                }
            }
            else
            {
                iPadRumbleFx(p, r, time_passed);
                break;
            }
        }
    }

    int ret = iPadUpdate(p, &new_on);

    if (!ret)
    {
        return 1;
    }

    if (p->flags & 0x10)
    {
        if (p->flags & 0x1)
        {
            unsigned int fake_dpad = 0;

            if (p->analog1.x >= 50)
            {
                fake_dpad |= XPAD_BUTTON_RIGHT;
            }
            else if (p->analog1.x <= -50)
            {
                fake_dpad |= XPAD_BUTTON_LEFT;
            }

            if (p->analog1.y >= 50)
            {
                fake_dpad |= XPAD_BUTTON_DOWN;
            }
            else if (p->analog1.y <= -50)
            {
                fake_dpad |= XPAD_BUTTON_UP;
            }

            if (fake_dpad == 0)
            {
                p->al2d_timer = 0.0f;
            }
            else
            {
                p->al2d_timer -= time_passed;

                if (p->al2d_timer <= 0.0f)
                {
                    new_on |= fake_dpad;
                    p->al2d_timer = XPAD_ANALOG_REPEAT_DELAY;
                }
            }
        }

        if (p->flags & 0x2)
        {
            if (p->analog2.x > -50 && p->analog2.x < 50 &&
                p->analog2.y > -50 && p->analog2.y < 50)
            {
                p->ar2d_timer = 0.0f;
            }
            else
            {
                p->ar2d_timer -= time_passed;

                if (p->ar2d_timer <= 0.0f)
                {
                    p->ar2d_timer = XPAD_ANALOG_REPEAT_DELAY;

                    if (p->analog2.x >= 50)
                    {
                        new_on |= XPAD_BUTTON_RIGHT;
                    }
                    else if (p->analog2.x <= -50)
                    {
                        new_on |= XPAD_BUTTON_LEFT;
                    }

                    if (p->analog2.y >= 50)
                    {
                        new_on |= XPAD_BUTTON_DOWN;
                    }
                    else if (p->analog2.y <= -50)
                    {
                        new_on |= XPAD_BUTTON_UP;
                    }
                }
            }
        }
    }

    p->pressed = new_on & ~p->on;
    p->released = p->on & ~new_on;
    p->on = new_on;

    for (int i = 0; i < 22; i++)
    {
        if (p->pressed & (1 << i))
        {
            p->down_tmr[i] = 0.0f;
        }
        else if (p->pressed & (1 << i))
        {
            p->up_tmr[i] = 0.0f;
        }

        if (p->on & (1 << i))
        {
            p->down_tmr[i] += time_passed;
        }
        else
        {
            p->up_tmr[i] += time_passed;
        }
    }

    if (p->flags & 0x10)
    {
        if (!(p->on & XPAD_BUTTON_UP) &&
            !(p->on & XPAD_BUTTON_DOWN) &&
            !(p->on & XPAD_BUTTON_LEFT) &&
            !(p->on & XPAD_BUTTON_RIGHT))
        {
            p->d_timer = 0.0f;
        }
        else
        {
            p->d_timer -= time_passed;

            if (p->d_timer <= 0.0f)
            {
                p->d_timer = XPAD_ANALOG_REPEAT_DELAY;

                if (p->on & XPAD_BUTTON_UP)
                {
                    p->pressed |= XPAD_BUTTON_UP;
                }
                else if (p->on & XPAD_BUTTON_DOWN)
                {
                    p->pressed |= XPAD_BUTTON_DOWN;
                }
            }

            if (p->on & XPAD_BUTTON_LEFT)
            {
                p->pressed |= XPAD_BUTTON_LEFT;
            }
            else if (p->on & XPAD_BUTTON_RIGHT)
            {
                p->pressed |= XPAD_BUTTON_RIGHT;
            }
        }
    }

    return 1;
}

void xPadKill()
{
    BFBBSTUB("xPadKill");
}

void xPadAnalogIsDigital(int idx, int enable)
{
    if (idx != 0)
    {
        return;
    }

    _tagxPad *p = &mPad[idx];

    if (enable)
    {
        p->flags |= 0x10;
    }
    else
    {
        p->flags &= ~0x10;
    }

    if (p->al2d_timer >= XPAD_ANALOG_REPEAT_DELAY)
    {
        p->al2d_timer = XPAD_ANALOG_REPEAT_DELAY;
    }

    if (p->ar2d_timer >= XPAD_ANALOG_REPEAT_DELAY)
    {
        p->ar2d_timer = XPAD_ANALOG_REPEAT_DELAY;
    }

    if (p->d_timer >= XPAD_ANALOG_REPEAT_DELAY)
    {
        p->d_timer = XPAD_ANALOG_REPEAT_DELAY;
    }
}