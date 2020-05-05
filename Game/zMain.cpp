#include "zMain.h"

#include "zGlobals.h"
#include "iSystem.h"
#include "iTime.h"
#include "zVar.h"
#include "zAssetTypes.h"
#include "xutil.h"
#include "xstransvc.h"
#include "xFont.h"

#include <windows.h>

static basic_rect<float> screen_bounds = { 0.0f, 0.0f, 1.0f, 1.0f };

zGlobals globals;
xGlobals *xglobals = &globals;

static void zMainOutputMgrSetup();
static void zMainMemLvlChkCB();
static void zMainInitGlobals();
static void zMainLoadFontHIP();

int main(int argc, char **argv)
{
    unsigned int options;

    memset(&globals, 0, sizeof(globals));
    globals.firstStartPressed = 1;

    options = 0;
    iSystemInit(options);

    zMainOutputMgrSetup();
    xMemRegisterBaseNotifyFunc(zMainMemLvlChkCB);
    zMainInitGlobals();
    var_init();
    zAssetStartup();
    zMainLoadFontHIP();
    xfont::init();

    zMainFirstScreen(1);

    return 0;
}

static void zMainOutputMgrSetup()
{
    iTimeDiffSec(iTimeGet());
    iTimeGet();
}

static void zMainInitGlobals()
{
    memset(&globals, 0, sizeof(globals));
    globals.sceneFirst = 1;

    iTimeDiffSec(iTimeGet());
    iTimeGet();
}

static void zMainMemLvlChkCB()
{
    zSceneMemLvlChkCB();
}

void zMainFirstScreen(int mode)
{
    RwCamera *cam = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    RwRGBA bg = { 0, 0, 0, 0 };

    for (int i = 0; i < 2; i++)
    {
        RwCameraClear(cam, &bg, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

        RwCameraBeginUpdate(cam);

        if (mode != 0)
        {
            xfont font = xfont::create(1, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f,
                                       { 255, 230, 0, 200 }, screen_bounds);
            xtextbox textbox = xtextbox::create(font, screen_bounds, 0x2,
                                                0.0f, 0.0f, 0.0f, 0.0f);

            textbox.set_text(
                "Game and Software © 2003 THQ Inc. © 2003 Viacom International Inc. "
                "All rights reserved.\n\n"
                "Nickelodeon, SpongeBob SquarePants and all related titles, logos, and "
                "characters are trademarks of Viacom International Inc. Created by "
                "Stephen Hillenburg.\n\n"
                "Exclusively published by THQ Inc. Developed by Heavy Iron. Portions of "
                "this software are Copyright 1998 - 2003 Criterion Software Ltd. and "
                "its Licensors. THQ, Heavy Iron and the THQ logo are trademarks and/or "
                "registered trademarks of THQ Inc. All rights reserved.\n\n"
                "All other trademarks, logos and copyrights are property of their "
                "respective owners.");

            textbox.bounds = screen_bounds;
            textbox.bounds.contract(0.1f);
            textbox.bounds.h = textbox.yextent(true);
            textbox.bounds.y = -(0.5f * textbox.bounds.h - 0.5f);

            textbox.render(true);
        }

        RwCameraEndUpdate(cam);

        RwCameraShowRaster(cam, GetActiveWindow(), 0);
    }

    Sleep(3000);

    iCameraDestroy(cam);
}

static void zMainLoadFontHIP()
{
    iTime t;

    xMemPushBase();

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);

    xSTPreLoadScene('FONT', NULL, 0x1);

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);

    xSTQueueSceneAssets('FONT', 0x1);

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);

    while (xSTLoadStep('FONT') < 1.0f) { }

    xSTDisconnect('FONT', 0x1);

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    const char CLASS_NAME[] = "SpongeBob";
    WNDCLASS wc = {};
    HWND hwnd;

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    //wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(hInstance, IDC_ARROW);

    RegisterClass(&wc);

    RECT rect = {};
    rect.left = 0;
    rect.top = 0;
    rect.right = SCREEN_WIDTH;
    rect.bottom = SCREEN_HEIGHT;

    if (!AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE))
    {
        return 0;
    }

    rect.right -= rect.left;
    rect.bottom -= rect.top;

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "SpongeBob SquarePants: Battle for Bikini Bottom",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);

    return main(__argc, __argv);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_PAINT:
        {
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}