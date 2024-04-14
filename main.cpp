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
    UserNotifier tray(LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON)), L"HideTeamsInMeetPopup v1.2.3 is running.\nClick on this icon to close.");
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
    LONG styles = GetWindowLongA(hWindow, GWL_STYLE);
    bool isMainWindow = (styles & WS_THICKFRAME) && (styles & WS_MAXIMIZEBOX);

    if (isMainWindow)
        return false;

    char className[100];
    GetClassNameA(hWindow, className, 100);

    bool isUsingNewTeams = 0 == strncmp(className, "TeamsWebView", 12) || NULL != FindWindowA("TeamsWebView", NULL);

    HWND firstChild = GetWindow(hWindow, GW_CHILD);

    if (isUsingNewTeams)
    {
        char firstChildClass[100];
        GetClassNameA(firstChild, firstChildClass, 100);
        
        return 0 == strncmp(firstChildClass, "TeamsOverlay", 12) || 0 == strncmp(firstChildClass, "Chrome_WidgetWin_0", 18);
    }
    else
    {
        char firstChildTitle[100];
        GetWindowTextA(firstChild, firstChildTitle, 100);

        return 0 == strncmp(className, "Chrome_WidgetWin_1", 18) && 0 == strncmp(firstChildTitle, "Chrome Legacy Window", 20);
    }

    return false;
}

void HideTeamsInMeetPopup(HWND hPopup)
{
    if (hPopup)
    {
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
