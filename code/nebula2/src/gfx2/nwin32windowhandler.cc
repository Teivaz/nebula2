#ifdef __WIN32__
//------------------------------------------------------------------------------
//  nwin32windowhandler.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx2/nwin32windowhandler.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
/**
*/
nWin32WindowHandler::nWin32WindowHandler() :
    hInst(0),
    hWnd(0),
    parentHwnd(0),
    hAccel(0),
    windowedStyle(WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE),
    fullscreenStyle(WS_POPUP | WS_SYSMENU | WS_VISIBLE),
    childStyle(WS_CHILD | WS_TABSTOP)
{
    // get applications module handle
    this->hInst = GetModuleHandle(0);
}

//------------------------------------------------------------------------------
/**
*/
nWin32WindowHandler::~nWin32WindowHandler()
{
    this->hInst = 0;
}

//------------------------------------------------------------------------------
/**
    Register window class, create accelerator table and create a minimized
    window.
*/
bool
nWin32WindowHandler::OpenWindow()
{
    n_assert(!this->windowOpen);
    n_assert(this->hInst);

    // add for parent HWND handling
    // check if an environment variable named "/sys/env/parent_hwnd" exists
    nAutoRef<nEnv> parent_hwnd("/sys/env/parent_hwnd");
    if (parent_hwnd.isvalid())
    {
        // parent window exist and set window height and width
        this->parentHwnd = (HWND) parent_hwnd->GetI();

        // we are a child, so set dimension from parent
        RECT r;
        GetClientRect(this->parentHwnd, &r);
        this->displayMode.SetWidth(r.right - r.left);
        this->displayMode.SetHeight(r.bottom - r.top);
        this->displayMode.SetType(nDisplayMode2::ChildWindow);
    }
    else
    {
        // parent window doesn't exist
        this->parentHwnd = NULL;
    }

    // initialize accelerator keys
    ACCEL acc[1];
    acc[0].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[0].key   = VK_RETURN;
    acc[0].cmd   = ACCEL_TOGGLEFULLSCREEN;
    this->hAccel = CreateAcceleratorTable(acc, 1);

    // initialize application icon
    HICON icon;
    icon = LoadIcon(NULL, IDI_APPLICATION);

    // initialize wndclass structure and call RegisterClass()
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.style         = 0;
    wc.lpfnWndProc   = WinProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(void*);   // used to hold 'this' pointer
    wc.hInstance     = this->hInst;
    wc.hIcon         = icon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "Nebula2 window class";
    wc.hIconSm       = NULL;
    RegisterClassEx(&wc);

    // open the window
    DWORD windowStyle = (((this->parentHwnd != 0) ? this->childStyle : this->windowedStyle) | WS_MINIMIZE);
    this->hWnd = CreateWindow("Nebula2 window class",                   // lpClassName
                              this->displayMode.GetWindowTitle().Get(), // lpWindowName
                              windowStyle,                              // dwStyle
                              0,                                        // x
                              0,                                        // y
                              0,                                        // nWidth
                              0,                                        // nHeight
                              this->parentHwnd,                         // hWndParent
                              NULL,                                     // hMenu
                              this->hInst,                              // hInstance
                              NULL);                                    // lpParam
    n_assert(this->hWnd);

    // initialize the user data field with this object's this pointer,
    // WndProc uses the user data field so that WinProc has access
    SetWindowLong(this->hWnd, 0, (LONG)this);

    // publish the window handle under a well defined name
    nEnv *env;
    if ((env = (nEnv *) nKernelServer::Instance()->New("nenv","/sys/env/hwnd")))
    {
        env->SetI((int)this->hWnd);
    }

    // minimize the window
    this->windowOpen       = true;
    this->windowMinimized  = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Destroy the window and accelerator table, unregister the window class.
*/
void
nWin32WindowHandler::CloseWindow()
{
    n_assert(this->windowOpen);
    n_assert(this->hInst);

    // close the window (if not already happened), the window may be
    // closed externally by Alt-F4 (for instance)
    if (this->hWnd)
    {
        n_assert(this->windowOpen);
        DestroyWindow(this->hWnd);
        this->hWnd = 0;
        this->windowOpen = false;
    }

    // release accelerator table
    if (this->hAccel)
    {
        DestroyAcceleratorTable(this->hAccel);
        this->hAccel = 0;
    }

    // unregister the window class
    if (!UnregisterClass("Nebula2 window class", this->hInst))
    {
        n_error("nWin32WindowHandler::CloseWindow(): UnregisterClass() failed!\n");
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nWin32WindowHandler::SetWindowTitle(const char* title)
{
    this->displayMode.SetWindowTitle(title);
    SetWindowText(this->hWnd, title);
}

//-----------------------------------------------------------------------------
/**
    Adjust window attributes to prepare for a new display mode.
*/
void
nWin32WindowHandler::AdjustWindowForChange()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    if (this->displayMode.GetType() == nDisplayMode2::Fullscreen)
    {
        // adjust for fullscreen mode
        SetWindowLong(this->hWnd, GWL_STYLE, this->fullscreenStyle);
    }
    else if (this->displayMode.GetType() == nDisplayMode2::ChildWindow)
    {
        // adjust for child mode
        SetWindowLong(this->hWnd, GWL_STYLE, this->childStyle);
    }
    else
    {
        // adjust for windowed mode
        SetWindowLong(this->hWnd, GWL_STYLE, this->windowedStyle);
    }
}

//-----------------------------------------------------------------------------
/**
    Restore the minimized app window to the size requested by SetDisplayMode(),

    Sets windowMinimized to false.

    - 06-Feb-04 floh    added icon handling
*/
void
nWin32WindowHandler::RestoreWindow()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);

    // set window title
    SetWindowText(this->hWnd, this->displayMode.GetWindowTitle().Get());

    // update icon (if exists)
    const nString& iconName = this->displayMode.GetIcon();
    if (iconName.IsValid())
    {
        HICON icon = LoadIcon(this->hInst, iconName.Get());
        if (icon)
        {
            SetClassLong(this->hWnd, GCL_HICON, (LONG)icon);
        }
    }

    // switch from minimized to fullscreen mode
    ShowWindow(this->hWnd, SW_RESTORE);

    int x, y, w, h;
    if (this->displayMode.GetType() == nDisplayMode2::Fullscreen)
    {
        x = 0;
        y = 0;
        w = this->displayMode.GetWidth();
        h = this->displayMode.GetHeight();
    }
    else if (this->displayMode.GetType() == nDisplayMode2::ChildWindow && this->parentHwnd)
    {
        RECT parentRect;
        // We are child window, so get dimesion from parent
        GetClientRect(this->parentHwnd, &parentRect);

        //calculate the desired window size
        RECT r = {0, 0, this->displayMode.GetWidth(), this->displayMode.GetHeight()};
        AdjustWindowRect(&r, this->childStyle, 0);

        x = this->displayMode.GetXPos();
        y = this->displayMode.GetYPos();
        if (x + (r.right - r.left) > (parentRect.right - parentRect.left))
        {
            w = (parentRect.right - parentRect.left) - x; //scale to max size inside the parrent window
        }
        else
        {
            w = (r.right - r.left);
        }

        if (y + (r.bottom - r.top) > (parentRect.bottom - parentRect.top))
        {
            h = parentRect.bottom - parentRect.top - y; //scale to max size inside the parrent window
        }
        else
        {
            h = r.bottom - r.top;
        }

        //update the displaymode
        this->displayMode.SetWidth(w);
        this->displayMode.SetHeight(h);
    }
    else
    {
        // Need to adjust w & h so that the *client* area
        // is equal to renderWidth/Height.
        RECT r = {0, 0, this->displayMode.GetWidth(), this->displayMode.GetHeight()};
        AdjustWindowRect(&r, this->windowedStyle, 0);
        x = this->displayMode.GetXPos();
        y = this->displayMode.GetYPos();
        w = r.right - r.left;
        h = r.bottom - r.top;
    }

    if (this->displayMode.GetType() == nDisplayMode2::AlwaysOnTop)
    {
        SetWindowPos(this->hWnd,            // the window handle
                    HWND_TOPMOST,           // placement order
                    x,                      // x position
                    y,                      // y position
                    w,                      // adjusted width
                    h,                      // adjusted height
                    SWP_SHOWWINDOW);
    }
    else if (this->displayMode.GetType() == nDisplayMode2::ChildWindow)
    {
        SetWindowPos(this->hWnd,            // the window handle
                    this->parentHwnd,       // placement order
                    x,                      // x position
                    y,                      // y position
                    w,                      // adjusted width
                    h,                      // adjusted height
                    SWP_SHOWWINDOW);
        //FIXME: For some reason, SetWindowPos doesn't work when in child mode
        MoveWindow(this->hWnd, x, y, w, h, 0);
    }
    else
    {
        SetWindowPos(this->hWnd,            // the window handle
                    HWND_NOTOPMOST,         // placement order
                    x,                      // x position
                    y,                      // y position
                    w,                      // adjusted width
                    h,                      // adjusted height
                    SWP_SHOWWINDOW);
    }
    this->windowMinimized = false;
}

//-----------------------------------------------------------------------------
/**
    Minimize the app window.

    Sets windowMinimized to true.
*/
void
nWin32WindowHandler::MinimizeWindow()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    if (!this->windowMinimized)
    {
        // minimize window for all mode except child
        if (this->displayMode.GetType() != nDisplayMode2::ChildWindow)
        {
            ShowWindow(this->hWnd, SW_MINIMIZE);
        }
        this->windowMinimized = true;
    }
}

//-----------------------------------------------------------------------------
/**
    The Trigger() method is called once per frame by nGfxServer2::Trigger().
    It should return false if the application should shutdown (for instance
    after an Alt-F4 has been received).

    - 24-Nov-04 kims added IsSkipMsgLoop() to skip message loop.
*/
bool
nWin32WindowHandler::Trigger()
{
    // just skip message loop such a case of the window is embedded
    // to other system like intergating Nebula with wxWindow.
    if (IsSkipMsgLoop())
        return true;

    // handle all pending WM's
    MSG msg;

    // if exist parent window, this window is in child mode
    if (this->GetDisplayMode().GetType() == nDisplayMode2::ChildWindow)
    {
        if (PeekMessage(&msg, NULL, WM_SIZE, WM_SIZE, PM_NOREMOVE))
        {
            int w = LOWORD(msg.lParam);
            int h = HIWORD(msg.lParam);
            ///FIXME: whats to do with this w,h vars?
        }
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            int msgHandled = false;
            if (this->hWnd && this->hAccel)
            {
                msgHandled = TranslateAccelerator(this->hWnd, this->hAccel, &msg);
            }
            if (!msgHandled)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return (!this->quitRequested);
}

//------------------------------------------------------------------------------
/**
    Override this method in a derived class!
*/
bool
nWin32WindowHandler::OnSetCursor()
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Override this method in a derived class!
*/
void
nWin32WindowHandler::OnPaint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Override this method in a derived class!
*/
void
nWin32WindowHandler::OnToggleFullscreenWindowed()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32WindowHandler::OnSize(bool minimize)
{
    if (minimize)
    {
        this->windowMinimized = true;
        if (this->refInputServer.isvalid())
        {
            this->refInputServer->LoseFocus();
        }
    }
    else
    {
        this->windowMinimized = false;
        if (this->refInputServer.isvalid())
        {
            this->refInputServer->ObtainFocus();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Override this method in a derived class! OnClose() should return true
    if the application should quit, otherwise false.
*/
bool
nWin32WindowHandler::OnClose()
{
    return false;
}

//-----------------------------------------------------------------------------
/**
    The winproc, does the usual housekeeping, and also generates mouse and keyboard
    input events.
*/
LONG
WINAPI
nWin32WindowHandler::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG retval = 1;

    // user data of windows contains 'this' pointer
    nWin32WindowHandler* self = (nWin32WindowHandler*) GetWindowLong(hWnd, 0);

    switch(uMsg)
    {
        case WM_SYSCOMMAND:
            // prevent moving/sizing and power loss in fullscreen mode
            if (self && (self->GetDisplayMode().GetType() == nDisplayMode2::Fullscreen))
            {
                switch (wParam)
                {
                    case SC_MOVE:
                    case SC_SIZE:
                    case SC_MAXIMIZE:
                    case SC_KEYMENU:
                    case SC_MONITORPOWER:
                        return 1;
                        break;
                }
            }
            break;

        case WM_ERASEBKGND:
            // prevent windows from erasing
            return 1;

        case WM_SIZE:
            if (self)
            {
                // inform input server about focus change
                if ((SIZE_MAXHIDE == wParam) || (SIZE_MINIMIZED == wParam))
                {
                    // let window handler react on size change
                    self->OnSize(true);

                }
                else
                {
                    // let window handler react on size change
                    self->OnSize(false);
                }
                ReleaseCapture();
            }
            break;

        case WM_SETCURSOR:
            if (self)
            {
                bool result = self->OnSetCursor();
                if (result)
                {
                    return TRUE;
                }
            }
            break;

        case WM_PAINT:
            if (self)
            {
                self->OnPaint();
            }
            break;

        case WM_SETFOCUS:
            // tell input server that we have obtained the input focus
            if (self && self->refInputServer.isvalid())
            {
                self->refInputServer->ObtainFocus();
                ReleaseCapture();
            }
            break;

        case WM_KILLFOCUS:
            // tell input server that we have lost the input focus
            if (self && self->refInputServer.isvalid())
            {
                self->refInputServer->LoseFocus();
                ReleaseCapture();
            }
            break;

        case WM_CLOSE:
            // ask Nebula to quit, everything else should happen in the destructor
            // If we're not a child window
            if (!self->GetParentHwnd())
            {
                if (self->OnClose())
                {
                    self->quitRequested = true;
                }
                return 0;
            }
            break;

        case WM_COMMAND:
            if (self)
            {
                switch (LOWORD(wParam))
                {
                    case nWin32WindowHandler::ACCEL_TOGGLEFULLSCREEN:
                    {
                        self->OnToggleFullscreenWindowed();
                    }
                    break;
                }
            }
            break;

        case WM_KEYDOWN:
            if (self && self->refInputServer.isvalid())
            {
                nKey key = self->TranslateKey((int)wParam);
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    ie->SetType(N_INPUT_KEY_DOWN);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(key);
                    self->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_SYSKEYDOWN:
            /* FIXME: this code prevents Alt-F4 from being handled! NOT GOOD!
            if (self && self->refInputServer.isvalid())
            {
                nKey key = self->TranslateKey((int)wParam);
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    ie->SetType(N_INPUT_KEY_DOWN);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(key);
                    self->refInputServer->LinkEvent(ie);
                }

                return(0);
            }
            */
            break;

        case WM_KEYUP:
            if (self && self->refInputServer.isvalid())
            {
                nKey key = self->TranslateKey((int)wParam);
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    ie->SetType(N_INPUT_KEY_UP);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(key);
                    self->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_SYSKEYUP:
            /* FIXME: this code prevents Alt-F4 from being handled! NOT GOOD!
            if (self && self->refInputServer.isvalid())
            {
                nKey key = self->TranslateKey((int)wParam);
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    ie->SetType(N_INPUT_KEY_UP);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(key);
                    self->refInputServer->LinkEvent(ie);
                }

                return(0);
            }
            */
            break;

        case WM_CHAR:
            if (self && self->refInputServer.isvalid())
            {
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    ie->SetType(N_INPUT_KEY_CHAR);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetChar((int) wParam);
                    self->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            if (self->GetParentHwnd())
            {
                SetFocus(hWnd);
            }
            if (self && self->refInputServer.isvalid())
            {
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    short x = LOWORD(lParam);
                    short y = HIWORD(lParam);
                    switch (uMsg)
                    {
                        case WM_LBUTTONDBLCLK:
                        case WM_RBUTTONDBLCLK:
                        case WM_MBUTTONDBLCLK:
                            ie->SetType(N_INPUT_BUTTON_DBLCLCK);
                            break;

                        case WM_LBUTTONDOWN:
                        case WM_RBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                            SetCapture(hWnd);
                            ie->SetType(N_INPUT_BUTTON_DOWN);
                            break;

                        case WM_LBUTTONUP:
                        case WM_RBUTTONUP:
                        case WM_MBUTTONUP:
                            ReleaseCapture();
                            ie->SetType(N_INPUT_BUTTON_UP);
                            break;
                    }
                    switch (uMsg)
                    {
                        case WM_LBUTTONDBLCLK:
                        case WM_LBUTTONDOWN:
                        case WM_LBUTTONUP:
                            ie->SetButton(0);
                            break;

                        case WM_RBUTTONDBLCLK:
                        case WM_RBUTTONDOWN:
                        case WM_RBUTTONUP:
                            ie->SetButton(1);
                            break;

                        case WM_MBUTTONDBLCLK:
                        case WM_MBUTTONDOWN:
                        case WM_MBUTTONUP:
                            ie->SetButton(2);
                            break;
                    }
                    ie->SetDeviceId(N_INPUT_MOUSE(0));
                    ie->SetAbsPos(x, y);
                    float relX = float(x) / self->GetDisplayMode().GetWidth();
                    float relY = float(y) / self->GetDisplayMode().GetHeight();
                    ie->SetRelPos(relX, relY);
                    self->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_MOUSEMOVE:
            if (self && self->refInputServer.isvalid())
            {
                short x = LOWORD(lParam);
                short y = HIWORD(lParam);
                nInputEvent *ie = self->refInputServer->NewEvent();
                if (ie)
                {
                    ie->SetType(N_INPUT_MOUSE_MOVE);
                    ie->SetDeviceId(N_INPUT_MOUSE(0));
                    ie->SetAbsPos(x, y);
                    float relX = float(x) / self->GetDisplayMode().GetWidth();
                    float relY = float(y) / self->GetDisplayMode().GetHeight();
                    ie->SetRelPos(relX, relY);
                    self->refInputServer->LinkEvent(ie);
                }
                self->OnMouseMove((UINT)x, (UINT)y);
            }
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
/**
    Translate a win32 keycode into a Nebula keycode.

    -24-Aug-05    kims    Fixed the bug #128, Added additional key mapping.
                          Thanks Magon Fox and Chris Tencati for the pointing out this.
*/
nKey
nWin32WindowHandler::TranslateKey(int vkey)
{
    nKey nk;
    switch(vkey)
    {
        case VK_LBUTTON:    nk=N_KEY_LBUTTON; break;
        case VK_RBUTTON:    nk=N_KEY_RBUTTON; break;
        case VK_MBUTTON:    nk=N_KEY_MBUTTON; break;
        case VK_BACK:       nk=N_KEY_BACK; break;
        case VK_TAB:        nk=N_KEY_TAB; break;
        case VK_RETURN:     nk=N_KEY_RETURN; break;
        case VK_SHIFT:      nk=N_KEY_SHIFT; break;
        case VK_CONTROL:    nk=N_KEY_CONTROL; break;
        case VK_MENU:       nk=N_KEY_MENU; break;
        case VK_PAUSE:      nk=N_KEY_PAUSE; break;
        case VK_CAPITAL:    nk=N_KEY_CAPITAL; break;
        case VK_ESCAPE:     nk=N_KEY_ESCAPE; break;
        case VK_SPACE:      nk=N_KEY_SPACE; break;
        case VK_PRIOR:      nk=N_KEY_PRIOR; break;
        case VK_NEXT:       nk=N_KEY_NEXT; break;
        case VK_END:        nk=N_KEY_END; break;
        case VK_HOME:       nk=N_KEY_HOME; break;
        case VK_LEFT:       nk=N_KEY_LEFT; break;
        case VK_UP:         nk=N_KEY_UP; break;
        case VK_RIGHT:      nk=N_KEY_RIGHT; break;
        case VK_DOWN:       nk=N_KEY_DOWN; break;
        case VK_SELECT:     nk=N_KEY_SELECT; break;
        case VK_PRINT:      nk=N_KEY_PRINT; break;
        case VK_EXECUTE:    nk=N_KEY_EXECUTE; break;
        case VK_SNAPSHOT:   nk=N_KEY_SNAPSHOT; break;
        case VK_INSERT:     nk=N_KEY_INSERT; break;
        case VK_DELETE:     nk=N_KEY_DELETE; break;
        case VK_HELP:       nk=N_KEY_HELP; break;
        case '0':           nk=N_KEY_0; break;
        case '1':           nk=N_KEY_1; break;
        case '2':           nk=N_KEY_2; break;
        case '3':           nk=N_KEY_3; break;
        case '4':           nk=N_KEY_4; break;
        case '5':           nk=N_KEY_5; break;
        case '6':           nk=N_KEY_6; break;
        case '7':           nk=N_KEY_7; break;
        case '8':           nk=N_KEY_8; break;
        case '9':           nk=N_KEY_9; break;
        case 'A':           nk=N_KEY_A; break;
        case 'B':           nk=N_KEY_B; break;
        case 'C':           nk=N_KEY_C; break;
        case 'D':           nk=N_KEY_D; break;
        case 'E':           nk=N_KEY_E; break;
        case 'F':           nk=N_KEY_F; break;
        case 'G':           nk=N_KEY_G; break;
        case 'H':           nk=N_KEY_H; break;
        case 'I':           nk=N_KEY_I; break;
        case 'J':           nk=N_KEY_J; break;
        case 'K':           nk=N_KEY_K; break;
        case 'L':           nk=N_KEY_L; break;
        case 'M':           nk=N_KEY_M; break;
        case 'N':           nk=N_KEY_N; break;
        case 'O':           nk=N_KEY_O; break;
        case 'P':           nk=N_KEY_P; break;
        case 'Q':           nk=N_KEY_Q; break;
        case 'R':           nk=N_KEY_R; break;
        case 'S':           nk=N_KEY_S; break;
        case 'T':           nk=N_KEY_T; break;
        case 'U':           nk=N_KEY_U; break;
        case 'V':           nk=N_KEY_V; break;
        case 'W':           nk=N_KEY_W; break;
        case 'X':           nk=N_KEY_X; break;
        case 'Y':           nk=N_KEY_Y; break;
        case 'Z':           nk=N_KEY_Z; break;
        case VK_LWIN:       nk=N_KEY_LWIN; break;
        case VK_RWIN:       nk=N_KEY_RWIN; break;
        case VK_APPS:       nk=N_KEY_APPS; break;
        case VK_NUMPAD0:    nk=N_KEY_NUMPAD0; break;
        case VK_NUMPAD1:    nk=N_KEY_NUMPAD1; break;
        case VK_NUMPAD2:    nk=N_KEY_NUMPAD2; break;
        case VK_NUMPAD3:    nk=N_KEY_NUMPAD3; break;
        case VK_NUMPAD4:    nk=N_KEY_NUMPAD4; break;
        case VK_NUMPAD5:    nk=N_KEY_NUMPAD5; break;
        case VK_NUMPAD6:    nk=N_KEY_NUMPAD6; break;
        case VK_NUMPAD7:    nk=N_KEY_NUMPAD7; break;
        case VK_NUMPAD8:    nk=N_KEY_NUMPAD8; break;
        case VK_NUMPAD9:    nk=N_KEY_NUMPAD9; break;
        case VK_MULTIPLY:   nk=N_KEY_MULTIPLY; break;
        case VK_ADD:        nk=N_KEY_ADD; break;
        case VK_SEPARATOR:  nk=N_KEY_SEPARATOR; break;
        case VK_SUBTRACT:   nk=N_KEY_SUBTRACT; break;
        case VK_DECIMAL:    nk=N_KEY_DECIMAL; break;
        case VK_DIVIDE:     nk=N_KEY_DIVIDE; break;
        case VK_F1:         nk=N_KEY_F1; break;
        case VK_F2:         nk=N_KEY_F2; break;
        case VK_F3:         nk=N_KEY_F3; break;
        case VK_F4:         nk=N_KEY_F4; break;
        case VK_F5:         nk=N_KEY_F5; break;
        case VK_F6:         nk=N_KEY_F6; break;
        case VK_F7:         nk=N_KEY_F7; break;
        case VK_F8:         nk=N_KEY_F8; break;
        case VK_F9:         nk=N_KEY_F9; break;
        case VK_F10:        nk=N_KEY_F10; break;
        case VK_F11:        nk=N_KEY_F11; break;
        case VK_F12:        nk=N_KEY_F12; break;
        case VK_F13:        nk=N_KEY_F13; break;
        case VK_F14:        nk=N_KEY_F14; break;
        case VK_F15:        nk=N_KEY_F15; break;
        case VK_F16:        nk=N_KEY_F16; break;
        case VK_F17:        nk=N_KEY_F17; break;
        case VK_F18:        nk=N_KEY_F18; break;
        case VK_F19:        nk=N_KEY_F19; break;
        case VK_F20:        nk=N_KEY_F20; break;
        case VK_F21:        nk=N_KEY_F21; break;
        case VK_F22:        nk=N_KEY_F22; break;
        case VK_F23:        nk=N_KEY_F23; break;
        case VK_F24:        nk=N_KEY_F24; break;
        case VK_NUMLOCK:    nk=N_KEY_NUMLOCK; break;
        case VK_SCROLL:     nk=N_KEY_SCROLL; break;
        case VK_OEM_1:      nk=N_KEY_SEMICOLON; break;
        case VK_OEM_2:      nk=N_KEY_SLASH; break;
        case VK_OEM_3:      nk=N_KEY_TILDE; break;
        case VK_OEM_4:      nk=N_KEY_LEFTBRACKET; break;
        case VK_OEM_5:      nk=N_KEY_BACKSLASH; break;
        case VK_OEM_6:      nk=N_KEY_RIGHTBRACKET; break;
        case VK_OEM_7:      nk=N_KEY_QUOTE; break;
        case VK_OEM_COMMA:  nk=N_KEY_COMMA; break;
        case VK_OEM_MINUS:  nk=N_KEY_UNDERBAR; break;
        case VK_OEM_PERIOD: nk=N_KEY_PERIOD; break;
        case VK_OEM_PLUS:   nk=N_KEY_EQUALITY; break;
        default:            nk=N_KEY_NONE; break;
    }
    return nk;
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32WindowHandler::OnMouseMove(int x, int y)
{
}

#endif __WIN32__

