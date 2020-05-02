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

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "SpongeBob SquarePants: Battle for Bikini Bottom",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
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