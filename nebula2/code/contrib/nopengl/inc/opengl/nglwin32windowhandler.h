#ifndef N_GLWIN32WINDOWHANDLER_H
#define N_GLWIN32WINDOWHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nGLWin32WindowHandler
    @ingroup OpenGL

    @brief A window handler for the gl graphics server class.

    This just implements some OpenGL specific callbacks which are called
    from the nWin32WindowHandler's WinProc.
    
    2004 Haron
*/
#include "gfx2/nwin32windowhandler.h"

class nGLServer2;

//------------------------------------------------------------------------------
class nGLWin32WindowHandler : public nWin32WindowHandler
{
public:
    /// constructor
    nGLWin32WindowHandler(nGLServer2* ptr);
    /// destructor
    ~nGLWin32WindowHandler();
    /// called by WinProc on WM_SETCURSOR
    virtual bool OnSetCursor();
    /// called by WinProc on WM_PAINT
    virtual void OnPaint();
    /// called when fullscreen/windowed should be toggled
    virtual void OnToggleFullscreenWindowed();
    /// called when WM_CLOSE is received, return true if app should quit
    virtual bool OnClose();

private:
    nGLServer2* glServer;
};

//------------------------------------------------------------------------------
#endif    
