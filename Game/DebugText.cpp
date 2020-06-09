#include "DebugText.h"

#include "xFont.h"
#include "iSystem.h"
#include "zMain.h"
#include "zCamera.h"

#include <stdio.h>

static xfont sFont;
static xtextbox sDebugTextbox;

static unsigned int sFrameCount = 0;

void DebugText_Init()
{
    sFont = xfont::create(XFONT_ID_ARIAL, NSCREENX(14.0f), NSCREENY(17.0f), 0.0f,
                          { 255, 255, 255, 255 }, screen_bounds);

    sDebugTextbox = xtextbox::create(sFont, screen_bounds, 0x0, 0.0f, 0.0f, 0.0f, 0.0f);
}

void DebugText_Render()
{
    char text[256];

    xVec3 *campos = &globals.camera.mat.pos;

    xVec3 camypr;
    xMat3x3GetEuler(&globals.camera.mat, &camypr);

    xVec3 camrot;

    camrot.x = RAD2DEG(camypr.y);
    camrot.y = RAD2DEG(camypr.x);
    camrot.z = RAD2DEG(camypr.z);

    const char *noclip = zcam_debug ? "On" : "Off";

    sprintf(text,
            "Frame count: %d\n"
            "Noclip (TAB): %s\n"
            "Cam pos: %f %f %f\n"
            "Cam rot: %f %f %f\n",
            sFrameCount,
            noclip,
            campos->x, campos->y, campos->z,
            camrot.x, camrot.y, camrot.z);

    sDebugTextbox.set_text(text);
    sDebugTextbox.render(true);

    sFrameCount++;
}