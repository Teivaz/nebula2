#define N_IMPLEMENTS nD3D9Server
//------------------------------------------------------------------------------
//  nd3d9server_window.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "input/ninputserver.h"

LONG WINAPI nD3D9Server_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//------------------------------------------------------------------------------
/**
    Sets a new window title.
*/
void
nD3D9Server::SetWindowTitle(const char* title)
{
    n_assert(this->windowOpen);
    nGfxServer2::SetWindowTitle(title);
    SetWindowText(this->hWnd, title);
}

//------------------------------------------------------------------------------
/**
    Register window class, create accelerator table and create a minimized
    window.
*/
bool
nD3D9Server::WindowOpen()
{
    n_assert(!this->windowOpen);
    n_assert(this->hInst);

    // add for parent HWND handling
    // check if an environment variable named "/sys/env/parent_hwnd" exists
    nAutoRef<nEnv> parent_hwnd(this->kernelServer);
    parent_hwnd = "/sys/env/parent_hwnd";

    if (parent_hwnd.isvalid()) 
    {
        // parent window exist and set window height and width
        this->parentHWnd = (HWND)parent_hwnd.get()->GetI();
        // we are a child, so set dimension from parent
        RECT r;
        GetClientRect(this->parentHWnd, &r);
        this->displayMode.SetWidth((ushort)(r.right - r.left));
        this->displayMode.SetHeight((ushort)(r.bottom - r.top));
    } 
    else
    {
        // parent window doesn't exist
        this->parentHWnd = NULL;       
    }

    // initialize accelerator keys
    ACCEL acc[3];
    acc[0].fVirt = FALT|FNOINVERT|FVIRTKEY;
    acc[0].key   = VK_RETURN;
    acc[0].cmd   = ACCEL_TOGGLEFULLSCREEN;
    this->hAccel = CreateAcceleratorTable(acc, 1);

    // initialize application icon
    HICON icon;
    icon = LoadIcon(NULL, "nebula_icon");

    // initialize wndclass structure and call RegisterClass()
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.style         = 0;
    wc.lpfnWndProc   = nD3D9Server_WinProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(void *);   // used to hold 'this' pointer
    wc.hInstance     = this->hInst;
    wc.hIcon         = icon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "nD3D9Server window class";
    wc.hIconSm       = NULL;
    RegisterClassEx(&wc);

    // open the window
    // fix for create window 
    // if first time is window created set to not visible,
    // after all initialisation it's set to orginal style
    this->hWnd = CreateWindow("nD3D9Server window class",       // lpClassName
                                this->GetWindowTitle(),           // lpWindowName
                                this->parentHWnd != NULL ? this->childStyle : WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ,  // dwStyle
                                0,                                // x
                                0,                                // y
                                this->displayMode.GetWidth(),     // nWidth
                                this->displayMode.GetHeight(),    // nHeight
                                this->parentHWnd,                 // hWndParent
                                NULL,                             // hMenu
                                this->hInst,                      // hInstance
                                NULL);                            // lpParam
    n_assert(this->hWnd);

    // initialize the user data field with this object's this pointer,
    // WndProc uses the user data field to get a pointer to
    // the nD3D9Server object
    SetWindowLong(this->hWnd, 0, (LONG)this);
    
    // minimize the window
    ShowWindow(this->hWnd, SW_SHOWMINIMIZED);
    this->windowOpen       = true;
    this->windowMinimized  = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Destroy the window and accelerator table, unregister the window class.
*/
void
nD3D9Server::WindowClose()
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
    if (!UnregisterClass("nD3D9Server window class", this->hInst))
    {
        n_error("nD3D9Server::classUnregister(): UnregisterClass() failed!\n");
    }
}

//-----------------------------------------------------------------------------
/**
    Adjust window attributes to prepare creating a new device. Fullscreen/
    windowed modes require different window attributes. Must be called from 
    within DeviceOpen() before creating the d3d device.
*/
void 
nD3D9Server::AdjustWindowForChange()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    if (this->displayMode.GetType() == nDisplayMode2::FULLSCREEN)
    {
        // adjust for fullscreen mode
        SetWindowLong(this->hWnd, GWL_STYLE, this->fullscreenStyle);
    }
    else if (this->displayMode.GetType() == nDisplayMode2::CHILDWINDOWED)
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
    Must be called from within DeviceOpen() after creating the d3d device.

    Sets windowMinimized to false.
*/
void
nD3D9Server::RestoreWindow()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);

    // switch from minimized to fullscreen mode
    ShowWindow(this->hWnd, SW_RESTORE);
    
    int w, h;
    if (this->displayMode.GetType() == nDisplayMode2::WINDOWED)
    {
        // Need to adjust adjust w & h so that the *client* area
        // is equal to renderWidth/Height.
        RECT r = {0, 0, this->displayMode.GetWidth(), this->displayMode.GetHeight()};
        AdjustWindowRect(&r, this->windowedStyle, 0);
        w = r.right - r.left;
        h = r.bottom - r.top;
    }
    else if (this->displayMode.GetType() == nDisplayMode2::CHILDWINDOWED)
    {
        // We are child window, so get dimesion from parent
        RECT r;
        GetClientRect(this->parentHWnd, &r);
        AdjustWindowRect(&r, this->childStyle, 0);
        w = r.right - r.left;
        h = r.bottom - r.top;
        this->displayMode.SetWidth(w);
        this->displayMode.SetHeight(h);
    }
    else 
    {
        // if child mode is FullScreen
        w = this->displayMode.GetWidth();
        h = this->displayMode.GetHeight();
    }

    SetWindowPos(this->hWnd,            // the window handle
                 HWND_NOTOPMOST,        // placement order
                 0,                     // x position
                 0,                     // y position
                 w,                     // adjusted width
                 h,                     // adjusted height
                 SWP_SHOWWINDOW);

    if (this->displayMode.GetType() == nDisplayMode2::CHILDWINDOWED) 
    {
        // For some reason, SetWindowPos doesn't work when in child mode
        MoveWindow(this->hWnd, 0, 0, w, h, 0);
    }

    this->windowMinimized = false;
}

//-----------------------------------------------------------------------------
/**
    Minimize the app window. Must be called from within DeviceClose() after
    the d3d device has been destroyed.

    Sets windowMinimized to true.
*/
void 
nD3D9Server::MinimizeWindow()
{
    n_assert(this->hWnd);
    n_assert(this->windowOpen);
    n_assert(!this->windowMinimized);

    // minimize window for all mode except child
    if (this->displayMode.GetType() != nDisplayMode2::CHILDWINDOWED) 
    {
        ShowWindow(this->hWnd, SW_MINIMIZE);
    }

    this->windowMinimized = true;
}

//-----------------------------------------------------------------------------
/**
    The winproc, does the usual stuff, and also generates mouse and keyboard
    input events.
*/
LONG 
WINAPI 
nD3D9Server_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG retval = 1;

    // user data of windows contains 'this' pointer
    nD3D9Server *d3d9 = (nD3D9Server *) GetWindowLong(hWnd,0);

    switch(uMsg) 
    {
        case WM_SYSCOMMAND:
            // prevent moving/sizing and power loss in fullscreen mode
            if (d3d9 && (d3d9->displayMode.GetType() == nDisplayMode2::FULLSCREEN))
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
            // see if we become minimized
            if (d3d9)
            {
                if ((SIZE_MAXHIDE==wParam) || (SIZE_MINIMIZED==wParam))
                {
                    d3d9->windowMinimized = true;
                }
                else
                {
                    d3d9->windowMinimized = false;
                }
            }
            break;

/*
FIXME!
        case WM_SETCURSOR:
            // show/hide the system and/or custom d3d cursor
            if (d3d8)
            {
                FIXME!

                if (d3d8->d3dCursorShown)
                {
                    SetCursor(NULL);
                    d3d8->d3d8Dev->ShowCursor(TRUE);
                    return TRUE;
                }
                else if (d3d8->systemCursorShown)
                {
                    d3d8->d3d8Dev->ShowCursor(FALSE);
                }
                else
                {
                    SetCursor(NULL);
                    return TRUE;
                }
            }
            break;
*/

        case WM_PAINT:
            // flip the buffers out of order, to fill window
            // with something useful, but only in windowed mode
            if (d3d9 && (d3d9->displayMode.GetType() != nDisplayMode2::FULLSCREEN) && d3d9->d3d9Device) 
            {
                d3d9->d3d9Device->Present(0, 0, 0, 0);
            }
            break;

        case WM_SETFOCUS:
            // tell input server that we have obtained the input focus
            if (d3d9 && d3d9->refInputServer.isvalid())
            {
                d3d9->refInputServer->ObtainFocus();
            }
            break;

        case WM_KILLFOCUS:
            // tell input server that we have lost the input focus
            if (d3d9 && d3d9->refInputServer.isvalid())
            {
                d3d9->refInputServer->LoseFocus();
            }
            break;

        case WM_CLOSE:
            // ask Nebula to quit, everything else should happen in the destructor
            // If we're not a child window
            if (!d3d9->GetParentHWnd())
            {
                d3d9->quitRequested = true;
                return 0;
            }
            break;

        case WM_COMMAND:
            if (d3d9) 
            {
                switch (LOWORD(wParam)) 
                {
                    case nD3D9Server::ACCEL_TOGGLEFULLSCREEN:
                    {
                        // toggle fullscreen/windowed
                        nDisplayMode2 mode = d3d9->displayMode;
                        if (mode.GetType() == nDisplayMode2::FULLSCREEN)
                        {
                            mode.Set(nDisplayMode2::WINDOWED, mode.GetWidth(), mode.GetHeight(), mode.GetVerticalSync());
                        }
                        else
                        {
                            mode.Set(nDisplayMode2::FULLSCREEN, mode.GetWidth(), mode.GetHeight(), mode.GetVerticalSync());
                        }
                        d3d9->CloseDisplay();
                        d3d9->SetDisplayMode(mode);
                        d3d9->OpenDisplay();
                    }
                    break;
                }
            }
            break;

        case WM_KEYDOWN:
            if (d3d9 && d3d9->refInputServer.isvalid()) 
            {
                nKey key = d3d9->TranslateKey((int)wParam);
                nInputEvent *ie = d3d9->refInputServer->NewEvent();
                if (ie) 
                {
                    ie->SetType(N_INPUT_KEY_DOWN);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(key);
                    d3d9->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_KEYUP:
            if (d3d9 && d3d9->refInputServer.isvalid()) 
            {
                nKey key = d3d9->TranslateKey((int)wParam);
                nInputEvent *ie = d3d9->refInputServer->NewEvent();
                if (ie) 
                {
                    ie->SetType(N_INPUT_KEY_UP);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetKey(key);
                    d3d9->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_CHAR:
            if (d3d9 && d3d9->refInputServer.isvalid()) 
            {
                nInputEvent *ie = d3d9->refInputServer->NewEvent();
                if (ie) 
                {
                    ie->SetType(N_INPUT_KEY_CHAR);
                    ie->SetDeviceId(N_INPUT_KEYBOARD(0));
                    ie->SetChar((int) wParam);
                    d3d9->refInputServer->LinkEvent(ie);
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
            if (d3d9->GetParentHWnd()) 
            {
                SetFocus(hWnd);
            }
            if (d3d9 && d3d9->refInputServer.isvalid()) 
            {
                nInputEvent *ie = d3d9->refInputServer->NewEvent();
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
                    float relX = float(x) / d3d9->displayMode.GetWidth();
                    float relY = float(y) / d3d9->displayMode.GetHeight();
                    ie->SetRelPos(relX, relY);
                    d3d9->refInputServer->LinkEvent(ie);
                }
            }
            break;

        case WM_MOUSEMOVE:
            if (d3d9 && d3d9->refInputServer.isvalid()) 
            {
                short x = LOWORD(lParam);
                short y = HIWORD(lParam);
                nInputEvent *ie = d3d9->refInputServer->NewEvent();
                if (ie) 
                {
                    ie->SetType(N_INPUT_MOUSE_MOVE);
                    ie->SetDeviceId(N_INPUT_MOUSE(0));
                    ie->SetAbsPos(x, y);
                    float relX = float(x) / d3d9->displayMode.GetWidth();
                    float relY = float(y) / d3d9->displayMode.GetHeight();
                    ie->SetRelPos(relX, relY);
                    d3d9->refInputServer->LinkEvent(ie);
                }
                /* FIXME
                if ((d3d8->displayMode.GetType() == nDisplayMode2::FULLSCREEN) && d3d8->d3dCursorShown)
                {
                    // in fullscreen mode, update the cursor position myself
                    d3d8->d3d8Dev->SetCursorPosition((UINT)x, (UINT)y, D3DCURSOR_IMMEDIATE_UPDATE);
                    return TRUE;
                }
                */
            }
            break;
    }
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//------------------------------------------------------------------------------
/**
    Translate a win32 keycode into a Nebula keycode.
*/
nKey 
nD3D9Server::TranslateKey(int vkey)
{
    nKey nk;
    switch(vkey) {
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
        default:            nk=N_KEY_NONE; break;
    }
    return nk;
}
