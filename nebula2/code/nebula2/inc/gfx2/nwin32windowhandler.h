#ifndef N_WIN32WINDOWHANDLER_H
#define N_WIN32WINDOWHANDLER_H
#ifdef __WIN32__
//------------------------------------------------------------------------------
/**
    A window handler for Win32 platforms. Contains the window handling
    code both for the Direct3D and OpenGL graphics servers (if running
    under Windows).

    (C) 2004 RadonLabs GmbH
*/
#include "gfx2/nwindowhandler.h"

//------------------------------------------------------------------------------
class nWin32WindowHandler : public nWindowHandler
{
public:
    /// constructor
    nWin32WindowHandler();
    /// destructor
    ~nWin32WindowHandler();
    /// open the application window
    bool OpenWindow();
    /// close the application window
    void CloseWindow();
    /// prepare window for switching between windowd/fullscreen mode
    void AdjustWindowForChange();
    /// restore window from minimized state
    void RestoreWindow();
    /// minimize the window
    void MinimizeWindow();
    /// call this method per frame, returns false if app should shut down
    bool Trigger();
    /// get the app window's hWnd
    HWND GetHwnd() const;
    /// get parent hWnd
    HWND GetParentHwnd() const;
    /// called by WinProc on WM_SETCURSOR
    virtual bool OnSetCursor();
    /// called by WinProc on WM_PAINT
    virtual void OnPaint();
    /// called when fullscreen/windowed should be toggled
    virtual void OnToggleFullscreenWindowed();
    /// called when WM_SIZE is received
    virtual void OnSize(bool minimize);
    /// called when WM_CLOSE is received, return true if app should quit
    virtual bool OnClose();

    /// the WinProc
    static LONG WINAPI WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    // NOTE: public because WinProc needs access!

    /// translate win32 keycode into Nebula keycode
    nKey TranslateKey(int vkey);

    HINSTANCE hInst;                ///< application instance handle
    HWND hWnd;                      ///< handle of this window
    HWND parentHwnd;                ///< handle of parent window  (child mode)

    HACCEL hAccel;                  ///< the accelerator table
    DWORD windowedStyle;            ///< WS_* flags for windowed mode
    DWORD fullscreenStyle;          ///< WS_* flags for fullscreen mode
    DWORD childStyle;               ///< WS_* flags for child mode

    enum
    {
        ACCEL_TOGGLEFULLSCREEN = 1001,
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
HWND
nWin32WindowHandler::GetHwnd() const
{
    return this->hWnd;
}

//------------------------------------------------------------------------------
/**
*/
inline
HWND
nWin32WindowHandler::GetParentHwnd() const
{
    return this->parentHwnd;
}

//------------------------------------------------------------------------------
#endif // __WIN32__
#endif
