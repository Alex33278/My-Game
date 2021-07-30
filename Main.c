#pragma warning(push, 3)
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include "Main.h"
#pragma warning(pop)

#pragma warning(disable: 5045)
#pragma warning(disable: 28251)

HWND GameWindow;
BOOL GameIsRunning;
GameBitMap BackBuffer;
PerformanceData PerData;

INT WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, INT CmdShow) 
{
    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(PrevInstance);
    UNREFERENCED_PARAMETER(CmdLine);
    UNREFERENCED_PARAMETER(CmdShow);

    if (GameRunning() == TRUE)
    {

        MessageBoxA(NULL, "Another instance is running.", GameName, MB_ICONWARNING | MB_OK);
        goto Exit;
    }

    if (CreateMainGameWindow() != ERROR_SUCCESS)
    {
        goto Exit;
    }

    QueryPerformanceFrequency(&PerData.PerFrequency);

    BackBuffer.BitMapInfo.bmiHeader.biSize = sizeof(BackBuffer.BitMapInfo.bmiHeader);
    BackBuffer.BitMapInfo.bmiHeader.biWidth = GameResWidth;
    BackBuffer.BitMapInfo.bmiHeader.biHeight = GameResHeight;
    BackBuffer.BitMapInfo.bmiHeader.biBitCount = GameBPP;
    BackBuffer.BitMapInfo.bmiHeader.biCompression = BI_RGB;
    BackBuffer.BitMapInfo.bmiHeader.biPlanes = 1;
    BackBuffer.Memory = VirtualAlloc(NULL, GameCanvasMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (BackBuffer.Memory == NULL)
    {
        MessageBoxA(NULL, "Failed to allocate memory.", GameName, MB_ICONERROR | MB_OK);
        goto Exit;
    }

    MSG Message = { 0 };

    GameIsRunning = TRUE;

    while (GameIsRunning)
    {
        QueryPerformanceCounter(&PerData.FrameStart);
        while (PeekMessageA(&Message, GameWindow, 0, 0, PM_REMOVE))
        {
            DispatchMessageA(&Message);
        }

        ProccessPlayerInput();
        RenderGraphics();
        QueryPerformanceCounter(&PerData.FrameEnd);

        PerData.ElapsedMicrosecondsPerFrame.QuadPart = PerData.FrameEnd.QuadPart - PerData.FrameStart.QuadPart;
        PerData.ElapsedMicrosecondsPerFrame.QuadPart *= 1000000;
        PerData.ElapsedMicrosecondsPerFrame.QuadPart /= PerData.PerFrequency.QuadPart;

        Sleep(1);

        PerData.FramesRendered++;

        if (PerData.FramesRendered % CalAvgFpsEveryXFrames == 0)
        {
            char str[64] = { 0 };
            _snprintf_s(str, _countof(str), _TRUNCATE, "Elapsed microseconds: %lli\n", PerData.ElapsedMicrosecondsPerFrame.QuadPart);
            OutputDebugStringA(str);
        }
    }

Exit:
    return(0);
}

LRESULT CALLBACK MainWinProc(HWND WinHandle, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Msg)
    {
        case WM_CLOSE:
        {
            GameIsRunning = FALSE;
            PostQuitMessage(0);
            break;
        }
        default:
        {
            Result = DefWindowProc(WinHandle, Msg, WParam, LParam);
        }
    }
    return(Result);
}

DWORD CreateMainGameWindow(void)
{
    DWORD Result = ERROR_SUCCESS;
    WNDCLASSEXA WinClass = { 0 };

    WinClass.cbSize = sizeof(WNDCLASSEXA);
    WinClass.style = 0;
    WinClass.lpfnWndProc = MainWinProc;
    WinClass.cbClsExtra = 0;
    WinClass.cbWndExtra = 0;
    WinClass.hInstance = GetModuleHandleA(NULL);
    WinClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    WinClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    WinClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    WinClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
    WinClass.lpszMenuName = NULL;
    WinClass.lpszClassName = GameName "_Window_Class";

    // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (RegisterClassExA(&WinClass) == 0)
    {
        Result = GetLastError();
        MessageBoxA(NULL, "Failed to register window.", GameName, MB_ICONERROR | MB_OK);
        goto Exit;
    }

    GameWindow = CreateWindowEx(0, WinClass.lpszClassName, GameName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, GetModuleHandleA(NULL), NULL);

    if (GameWindow == NULL)
    {
        Result = GetLastError();
        MessageBoxA(NULL, "Failed to create window.", GameName, MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    PerData.MonitorInfo.cbSize = sizeof(MONITORINFO);

    if (GetMonitorInfoA(MonitorFromWindow(GameWindow, MONITOR_DEFAULTTOPRIMARY), &PerData.MonitorInfo) == 0)
    {
        Result = ERROR_MONITOR_NO_DESCRIPTOR;
        MessageBoxA(NULL, "Failed to get monitor info.", GameName, MB_ICONERROR | MB_OK);
        goto Exit;
    }

    PerData.MonitorWidth = PerData.MonitorInfo.rcMonitor.right - PerData.MonitorInfo.rcMonitor.left;
    PerData.MonitorHeight = PerData.MonitorInfo.rcMonitor.bottom - PerData.MonitorInfo.rcMonitor.top;

    if (SetWindowLongPtrA(GameWindow, GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW) == 0)
    {
        Result = GetLastError();
        MessageBoxA(GameWindow, "Failed to run 'SetWindowLongPtrA' function.", GameName, MB_ICONERROR | MB_OK);
        goto Exit;
    }
    if (SetWindowPos(GameWindow, HWND_TOP, PerData.MonitorInfo.rcMonitor.left, PerData.MonitorInfo.rcMonitor.top, PerData.MonitorWidth, PerData.MonitorHeight, SWP_FRAMECHANGED | SWP_NOOWNERZORDER) == 0)
    {
        Result = GetLastError();
        MessageBoxA(GameWindow, "Failed to run 'SetWindowPos' function.", GameName, MB_ICONERROR | MB_OK);
        goto Exit;
    }

Exit:
    return(Result);
}

BOOL GameRunning(void)
{
    HANDLE Mutex = NULL;
    Mutex = CreateMutexA(NULL, FALSE, GameName "_Mutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

void ProccessPlayerInput(void)
{
    int16_t escKeyDown = GetAsyncKeyState(VK_ESCAPE);

    if (escKeyDown)
    {
        SendMessageA(GameWindow, WM_CLOSE, 0, 0);
    }
}

void RenderGraphics(void)
{
    Pixel32 Pixel = { 0 };
    Pixel.Blue = 0xff;
    Pixel.Green = 0x7f;
    Pixel.Red = 0;

    for (int x = 0; x < GameResWidth * GameResHeight; x++)
    {
        memcpy_s((Pixel32*) BackBuffer.Memory + x, sizeof(Pixel32), &Pixel, sizeof(Pixel32));
    }

    HDC DeviceContext = GetDC(GameWindow);
    StretchDIBits(DeviceContext, 0, 0, PerData.MonitorWidth, PerData.MonitorHeight, 0, 0, GameResWidth, GameResHeight, BackBuffer.Memory, &BackBuffer.BitMapInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(GameWindow, DeviceContext);
}