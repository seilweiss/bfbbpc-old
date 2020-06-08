#include "iWindow.h"

#include "iSystem.h"

#include <windows.h>

extern int main(int argc, char **argv);

void iWindowUpdate()
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_PAINT)
        {
            break;
        }

        if (msg.message == WM_QUIT)
        {
            exit(0);
        }
    }
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    const char CLASS_NAME[] = "SpongeBob";
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    //wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

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

    HWND hwnd = CreateWindowEx(
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