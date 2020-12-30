#include <Windows.h>

#include "UserNotifier.h"
#include "resource.h"

const DWORD MAIN_THREAD_ID = GetCurrentThreadId();

bool IsWindowTeamsInMeetPopup(HWND hWindow);
void HideTeamsInMeetPopup(HWND hPopup);

BOOL CALLBACK FindTeamsInMeetPopup(HWND hwnd, LPARAM lParam);
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);

void onTrayIconClick(const POINT& pos)
{
    PostThreadMessage(MAIN_THREAD_ID, WM_QUIT, 0, 0);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{    
    UserNotifier tray(LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON)), L"HideTeamsInMeetPopup is running.\nLeft-Click on this icon to close.");    
    tray.SetOnLeftClick(&onTrayIconClick);

    HWND hPopup = NULL;
    EnumWindows(&FindTeamsInMeetPopup, (LPARAM)&hPopup);
    HideTeamsInMeetPopup(hPopup);

    if (SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE, NULL, &WinEventProc, 0, 0, 0))
    {
        MSG msg = {};
        while (GetMessage(&msg, 0, 0, 0)){}        
    }
    else
        MessageBoxA(NULL, "Error: Failed to start event hook !", "HideTeamsInMeetPopup - Error", MB_OK);
}

bool IsWindowTeamsInMeetPopup(HWND hWindow)
{
    char name[33];
    GetWindowTextA(hWindow, name, 33);
    
    return strncmp(name, "Microsoft Teams Call in progress", 32) == 0;
}

void HideTeamsInMeetPopup(HWND hPopup)
{
    if (hPopup)
    {
        CloseWindow(hPopup);
        ShowWindow(hPopup, SW_HIDE);
    }
}

BOOL CALLBACK FindTeamsInMeetPopup(HWND hwnd, LPARAM lParam)
{
    if (IsWindow(hwnd) && IsWindowVisible(hwnd) && IsWindowTeamsInMeetPopup(hwnd))
    {
        *((HWND*)lParam) = hwnd;
        return FALSE;
    }
    else
        return TRUE;
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
    char name[33];
    GetWindowTextA(hwnd, name, 33);

    if (event == EVENT_OBJECT_NAMECHANGE && IsWindowTeamsInMeetPopup(hwnd))
    {
        HideTeamsInMeetPopup(hwnd);
    }
}
