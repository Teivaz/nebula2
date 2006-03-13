#ifndef N_D3D9WINDOWHANDLER_H
#define N_D3D9WINDOWHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9WindowHandler
    @ingroup Gfx2

    A window handler for the d3d9 graphics server class.
    This just implements some Direct3D specific callbacks which are called
    from the nWin32WindowHandler's WinProc.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gfx2/nwin32windowhandler.h"

class nD3D9Server;

//------------------------------------------------------------------------------
class nD3D9WindowHandler : public nWin32WindowHandler
{
public:
    /// constructor
    nD3D9WindowHandler(nD3D9Server* ptr);
    /// destructor
    ~nD3D9WindowHandler();
    /// called by WinProc on WM_SETCURSOR
    virtual bool OnSetCursor();
    /// called by WinProc on WM_PAINT
    virtual void OnPaint();
    /// called when fullscreen/windowed should be toggled
    virtual void OnToggleFullscreenWindowed();
    /// called when WM_CLOSE is received, return true if app should quit
    virtual bool OnClose();
    /// called when WM_SIZE is received
    virtual void OnSize(bool minimize);
    /// called when WM_MOUSEMOVE is received
    virtual void OnMouseMove(int x, int y);

private:
    nD3D9Server* d3d9Server;
};

//------------------------------------------------------------------------------
#endif    