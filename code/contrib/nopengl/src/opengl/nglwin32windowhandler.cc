//------------------------------------------------------------------------------
//  nglwin32windowhandler.cc
//  2004 Haron
//------------------------------------------------------------------------------
#include "opengl/nglwin32windowhandler.h"
#include "opengl/nglserver2.h"

//------------------------------------------------------------------------------
/**
*/
nGLWin32WindowHandler::nGLWin32WindowHandler(nGLServer2* ptr) :
    glServer(ptr)
{
    n_assert(this->glServer);
}

//------------------------------------------------------------------------------
/**
*/
nGLWin32WindowHandler::~nGLWin32WindowHandler()
{
    this->glServer = 0;
}

//------------------------------------------------------------------------------
/**
    FIXME: this method directly accesses GLServer2 members!
*/
bool
nGLWin32WindowHandler::OnSetCursor()
{
    n_assert(this->glServer);
/*    if (this->d3d9Server->d3d9Device)
    {
        switch (this->d3d9Server->cursorVisibility)
        {
            case nGfxServer2::None:
                SetCursor(NULL);
                this->d3d9Server->d3d9Device->ShowCursor(FALSE);
                return true;

            case nGfxServer2::System:
                this->d3d9Server->d3d9Device->ShowCursor(FALSE);
                break;

            case nGfxServer2::Custom:
                SetCursor(NULL);
                this->d3d9Server->d3d9Device->ShowCursor(TRUE);
                return true;
        }
    }
*/    return false;
}

//------------------------------------------------------------------------------
/**
    FIXME: this method directly accesses GLServer2 members!
*/
void
nGLWin32WindowHandler::OnPaint()
{
    n_assert(this->glServer);
/*    if ((this->GetDisplayMode().GetType() != nDisplayMode2::Fullscreen) && this->d3d9Server->d3d9Device) 
    {
        if (!this->d3d9Server->InDialogBoxMode())
        {
            this->d3d9Server->d3d9Device->Present(0, 0, 0, 0);
        }
    }*/
}

//------------------------------------------------------------------------------
/**
*/
void
nGLWin32WindowHandler::OnToggleFullscreenWindowed()
{
    n_assert(this->glServer);

    // toggle fullscreen/windowed
    nDisplayMode2 newMode = this->GetDisplayMode();
    if (newMode.GetType() == nDisplayMode2::Fullscreen)
    {
        newMode.SetType(nDisplayMode2::Windowed);
    }
    else
    {
        newMode.SetType(nDisplayMode2::Fullscreen);
    }
    this->glServer->CloseDisplay();
    this->glServer->SetDisplayMode(newMode);
    this->glServer->OpenDisplay();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGLWin32WindowHandler::OnClose()
{
    n_assert(this->glServer);

    // *** HACK *** HACK *** HACK ***
/*
    int res = MessageBox(this->hWnd, "Really Quit?", "Hack Hack Hack", MB_OKCANCEL);
    return (res == IDOK);
*/
return true;

}
