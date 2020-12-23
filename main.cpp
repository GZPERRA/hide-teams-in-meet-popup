#include <Windows.h>

#include "UserNotifier.h"
#include "resource.h"

const DWORD MAIN_THREAD_ID = GetCurrentThreadId();

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);

void onTrayIconClick(const POINT& pos)
{
    PostThreadMessage(MAIN_THREAD_ID, WM_QUIT, 0, 0);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{    
    UserNotifier tray(LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON)), L"HideTeamsInMeetPopup is running.\nDouble-click this icon to close.");
    
    tray.SetOnLeftClick(&onTrayIconClick);

    if (SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE, NULL, &WinEventProc, 0, 0, 0))
    {
        MSG msg = {};
        while (GetMessage(&msg, 0, 0, 0)){}        
    }
    else
        MessageBoxA(NULL, "Error: Failed to start event hook !", "HideTeamsInMeetPopup - Error", MB_OK);
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
    char name[33];
    GetWindowTextA(hwnd, name, 33);

    if (event == EVENT_OBJECT_NAMECHANGE && strncmp(name, "Microsoft Teams Call in progress", 32) == 0)
    {
        SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW);
    }
}
