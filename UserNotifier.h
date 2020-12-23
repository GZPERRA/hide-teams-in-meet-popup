#pragma once
#include <shobjidl.h>

class UserNotifier : IUserNotificationCallback, IQueryContinue
{
    
public:
    enum class MessageIcon
    {
        NoIcon,
        Information,
        Warning,
        Critical
    };

    UserNotifier(HICON taskBarIcon = 0, LPCWSTR iconTooltip = 0);
    ~UserNotifier();

    void SetOnLeftClick(void(*onLeftClick)(const POINT&));
    void SetContextMenu(void(*onContextMenu)(const POINT&));

    void ShowMessage(LPCWSTR title, LPCWSTR message, const MessageIcon& messageIcon = MessageIcon::Information, DWORD interval = 1000);
    void ShowMessageSync(LPCWSTR title, LPCWSTR message, const MessageIcon& messageIcon = MessageIcon::Information, DWORD interval = 1000);

private:

    void UpdateBalloonInfo(LPCWSTR title, LPCWSTR message, const MessageIcon& messageIcon);

    bool mCoInitialized;
    bool mValid;
    bool mTerminateThread;

    IUserNotification2* mUserNotif;

    HANDLE mNotificationThread;
    HANDLE mThreadEvent;

    DWORD mInterval;
    HICON mIcon;
    LPCWSTR mIconTooltip;

    void(*mOnLeftClick)(const POINT&);   
    void(*mOnContextMenu)(const POINT&);

    static DWORD WINAPI NotificationThread(LPVOID notificationData);

    // Inherited via IUserNotificationCallback
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    virtual ULONG STDMETHODCALLTYPE AddRef() override;
    virtual ULONG STDMETHODCALLTYPE Release() override;
    virtual HRESULT STDMETHODCALLTYPE OnBalloonUserClick(POINT* pt) override;
    virtual HRESULT STDMETHODCALLTYPE OnLeftClick(POINT* pt) override;
    virtual HRESULT STDMETHODCALLTYPE OnContextMenu(POINT* pt) override;

    // Inherited via IQueryContinue
    virtual HRESULT STDMETHODCALLTYPE QueryContinue() override;
};