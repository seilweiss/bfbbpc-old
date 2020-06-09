#include "DebugText.h"

#include "xFont.h"
#include "iSystem.h"
#include "zMain.h"
#include "zCamera.h"
#include "iTime.h"

#include <stdio.h>

static xfont sFont;
static xtextbox sDebugTextbox;

static iTime sCurTime;

void DebugText_Init()
{
    sFont = xfont::create(XFONT_ID_ARIAL, NSCREENX(14.0f), NSCREENY(17.0f), 0.0f,
                          { 255, 255, 255, 255 }, screen_bounds);

    sDebugTextbox = xtextbox::create(sFont, screen_bounds, 0x0, 0.0f, 0.0f, 0.0f, 0.0f);

    sCurTime = iTimeGet();
}

#define FPS_AVG_SIZE 16
#define FPS_UPDATE_FREQ 0.1f

static float DebugText_GetFPS()
{
    static float fps[FPS_AVG_SIZE] = { 0 };
    static float *fps_ptr = &fps[0];

    iTime time_prev = sCurTime;
    sCurTime = iTimeGet();

    float tdiff = iTimeDiffSec(time_prev, sCurTime);

    *fps_ptr = 1.0f / xmax(tdiff, iTimeTicksToSecs(1));

    fps_ptr++;

    if (fps_ptr == &fps[FPS_AVG_SIZE])
    {
        fps_ptr = &fps[0];
    }

    static float fps_avg = 0.0f;
    static float fps_tmr = 0.0f;

    if (fps_tmr <= 0.0f)
    {
        fps_tmr = FPS_UPDATE_FREQ;
        fps_avg = 0.0f;

        for (int i = 0; i < FPS_AVG_SIZE; i++)
        {
            fps_avg += fps[i];
        }

        fps_avg /= FPS_AVG_SIZE;
    }
    else
    {
        fps_tmr -= tdiff;
    }

    return fps_avg;
}

void DebugText_Render()
{
    char text[256];

    int fps = (int)roundf(DebugText_GetFPS());

    xVec3 *campos = &globals.camera.mat.pos;

    xVec3 camypr;
    xMat3x3GetEuler(&globals.camera.mat, &camypr);

    xVec3 camrot;

    camrot.x = RAD2DEG(camypr.y);
    camrot.y = RAD2DEG(camypr.x);
    camrot.z = RAD2DEG(camypr.z);

    const char *noclip = zcam_debug ? "On" : "Off";

    sprintf(text,
            "FPS: %d\n"
            "Noclip (TAB): %s\n"
            "Cam pos: %f %f %f\n"
            "Cam rot: %f %f %f\n",
            fps,
            noclip,
            campos->x, campos->y, campos->z,
            camrot.x, camrot.y, camrot.z);

    sDebugTextbox.set_text(text);
    sDebugTextbox.render(true);
}