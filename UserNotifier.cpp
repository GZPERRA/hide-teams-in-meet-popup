#include "UserNotifier.h"

#include <iostream>

UserNotifier::UserNotifier(HICON taskBarIcon, LPCWSTR iconTooltip)
{
    mCoInitialized = false;
    mValid = false;
    mTerminateThread = false;

    mUserNotif = 0;
    
    mNotificationThread = 0;
    mThreadEvent = 0;
    
    mInterval = 0;
    mIcon = 0;
    mIconTooltip = 0;

    mOnLeftClick = 0;
    mOnContextMenu = 0;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (mCoInitialized = SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_UserNotification, NULL, CLSCTX_ALL, IID_IUserNotification2, reinterpret_cast<void**>(&mUserNotif));
        if (mValid = SUCCEEDED(hr))
        {
            if (taskBarIcon)            
                mUserNotif->SetIconInfo(taskBarIcon, iconTooltip);      

            mNotificationThread = CreateThread(0, 0, NotificationThread, reinterpret_cast<LPVOID>(this), 0, NULL);
        }
    }
}

UserNotifier::~UserNotifier()
{
    if (mValid)
    {
        mTerminateThread = true;
        
        WaitForSingleObject(mNotificationThread, INFINITE);

        mUserNotif->Release();

        CloseHandle(mNotificationThread);
    }

    if (mCoInitialized)
        CoUninitialize();
}

void UserNotifier::SetOnLeftClick(void(*onLeftClick)(const POINT& mousePos))
{
    mOnLeftClick = onLeftClick;
}

void UserNotifier::SetContextMenu(void(*onContextMenu)(const POINT&))
{
    mOnContextMenu = onContextMenu;
}

void UserNotifier::ShowMessage(LPCWSTR title, LPCWSTR message, const MessageIcon& messageIcon, DWORD interval)
{
    mInterval = interval;    
    UpdateBalloonInfo(title, message, messageIcon);
}

void UserNotifier::ShowMessageSync(LPCWSTR title, LPCWSTR message, const MessageIcon& messageIcon, DWORD interval)
{
    mInterval = interval;
    UpdateBalloonInfo(title, message, messageIcon);
    Sleep(interval);
}

void UserNotifier::UpdateBalloonInfo(LPCWSTR title, LPCWSTR message, const MessageIcon& messageIcon)
{
    DWORD dwInfoFlags = NIIF_NONE;

    switch (messageIcon)
    {
    case MessageIcon::Information:
        dwInfoFlags = NIIF_INFO;
        break;
    case MessageIcon::Warning:
        dwInfoFlags = NIIF_WARNING;
        break;
    case MessageIcon::Critical:
        dwInfoFlags = NIIF_ERROR;
        break;
    }

    mUserNotif->SetBalloonRetry(0, mInterval, 0);
    mUserNotif->SetBalloonInfo(title, message, dwInfoFlags);
}

DWORD WINAPI UserNotifier::NotificationThread(LPVOID lpCaller)
{
    UserNotifier* caller = reinterpret_cast<UserNotifier*>(lpCaller);
    
    caller->mUserNotif->Show(caller, caller->mInterval, caller);

    return 0;
}

HRESULT STDMETHODCALLTYPE UserNotifier::QueryInterface(REFIID riid, void** ppvObject)
{
    if (riid == IID_IUnknown || riid == IID_IUserNotificationCallback || riid == IID_IQueryContinue)
    {
        (*ppvObject) = this;
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}

ULONG STDMETHODCALLTYPE UserNotifier::AddRef()
{
    return 1;
}

ULONG STDMETHODCALLTYPE UserNotifier::Release()
{
    return 0;
}

HRESULT STDMETHODCALLTYPE UserNotifier::OnBalloonUserClick(POINT* pt)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UserNotifier::OnLeftClick(POINT* pt)
{    
    if (mOnLeftClick)
        mOnLeftClick(*pt);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UserNotifier::OnContextMenu(POINT* pt)
{
    if (mOnContextMenu)
        mOnContextMenu(*pt);        

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UserNotifier::QueryContinue()
{
    return mTerminateThread ? S_FALSE : S_OK;
}
