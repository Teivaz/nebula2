//------------------------------------------------------------------------------
//  nglserver2_main.cc
//  2003-cubejk
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"

#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"

#include "opengl/nglmesh.h"
#include "opengl/ngltexture.h"
//#include "opengl/nglextensionserver.h"

//#include "kernel/ntimeserver.h"

nNebulaClass(nGLServer2, "ngfxserver2");
nGLServer2* nGLServer2::Singleton = 0;

//-----------------------------------------------------------------------------
/**
    check for GL errors
*/
void
n_gltrace(const char *msg)
{
    //TODO: GL Error handling
    bool errorPresent = false;
    GLenum error;
    uint errNum = 0;
    nString message(msg);
    message.Append(" Error:\n");
    error = glGetError();
    while (error != GL_NO_ERROR)
    {
        errorPresent = true;
        if (errNum < 20)
        {
            message.Append("    FIXME ");
            switch (error)
            {
                case GL_OUT_OF_MEMORY:
                    message.Append("<GL_OUT_OF_MEM>\n");
                    break;
                case GL_INVALID_ENUM:
                    message.Append("<GL_INVALID_ENUM>\n");
                    break;
                case GL_INVALID_VALUE:
                    message.Append("<GL_INVALID_VALUE>\n");
                    break;
                case GL_INVALID_OPERATION:
                    message.Append("<GL_INVALID_OPERATION>\n");
                    break;
                default:
                    message.Append("<GL_ERROR_TYPE: ");
                    message.AppendInt(error);
                    message.Append(">\n");
            }
        }
        else
        {
            message.Append("    ...");
            break;
        }
        errNum++;
        error = glGetError();
    }
    if (errorPresent)
    {
        //n_error(message.Get());
        n_message(message.Get());
    }
}

//------------------------------------------------------------------------------
/**
*/
nGLServer2::nGLServer2() :
    #ifdef __NEBULA_STATS__
    timeStamp(0.0),
    //queryResourceManager(0),
    //dbgQueryTextureTrashing("gfxTexTrashing", nArg::Bool),
    //dbgQueryTextureApproxBytesDownloaded("gfxTexApproxBytesDownloaded", nArg::Int),
    //dbgQueryTextureNumEvicts("gfxTexNumEvicts", nArg::Int),
    //dbgQueryTextureNumVidCreates("gfxTexNumVidCreates", nArg::Int),
    //dbgQueryTextureLastPri("gfxTexLastPri", nArg::Int),
    //dbgQueryTextureNumUsed("gfxTexNumUsed", nArg::Int),
    //dbgQueryTextureNumUsedInVidMem("gfxTexNumUsedInVidMem", nArg::Int),
    //dbgQueryTextureWorkingSet("gfxTexWorkingSet", nArg::Int),
    //dbgQueryTextureWorkingSetBytes("gfxTexWorkingSetBytes", nArg::Int),
    //dbgQueryTextureTotalManaged("gfxTexTotalManaged", nArg::Int),
    //dbgQueryTextureTotalBytes("gfxTexTotalBytes", nArg::Int),
    //dbgQueryNumPrimitives("gfxNumPrimitives", nArg::Int),
    //dbgQueryFPS("gfxFPS", nArg::Float),
    //dbgQueryNumDrawCalls("gfxNumDrawCalls", nArg::Int),
    //dbgQueryNumRenderStateChanges("gfxNumRenderStateChanges", nArg::Int),
    //dbgQueryNumTextureChanges("gfxNumTextureChanges", nArg::Int),
    statsFrameCount(0),
    statsNumTextureChanges(0),
    statsNumRenderStateChanges(0),
    statsNumDrawCalls(0),
    statsNumPrimitives(0),
    #endif
    windowHandler(this),
#ifdef __WIN32__
    hDC(0),
    context(0),
/*    hInst(0),
    hWnd(0),
    parentHWnd(0),
    hAccel(0),
    windowedStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE),
    fullscreenStyle(WS_POPUP | WS_SYSMENU | WS_VISIBLE),
    windowOpen(false),
    windowMinimized(false),
    quitRequested(false),
    pixelFormat(-1),*/
#endif
#ifdef __LINUX__
    IsInit(false),ext_XF86VIDMODE(false),isFullscreen(false),
    display(NULL),screen(0),xvi(NULL),
    x(0),y(0),height(0),width(0),border(0),
    wm_win(0),ctx_win(0),fs_win(0),
    glx_ctx_win(0),glx_fs_win(0),
#endif
    featureSet(InvalidFeatureSet)
{
    n_assert(0 == Singleton);
    Singleton = this;

    WATCHER_INIT(watchNumPrimitives,         "watchGfxNumPrimitives", nArg::Int);
    WATCHER_INIT(watchFPS,                   "watchGfxFPS",           nArg::Float);
    WATCHER_INIT(watchNumDrawCalls,          "watchGfxDrawCalls",     nArg::Int);
    WATCHER_INIT(watchNumRenderStateChanges, "watchGfxRSChanges",     nArg::Int);

    // open the app window
    //this->windowHandler.OpenWindow();

    //nKernelServer::Instance()->New("nglextensionserver", "/sys/servers/glextension");

    //nGLExtensionServer::InitExtensions();

    // check required extensions
    //n_assert2(N_GL_EXTENSION_SUPPORTED(GL_ARB_shader_objects), "nGLServer2::nGLServer2(): GL_ARB_shader_objects extention not supported!");
    //n_assert2(N_GL_EXTENSION_SUPPORTED(GL_ARB_vertex_buffer_object), "nGLServer2::nGLServer2(): GL_ARB_vertex_buffer_object extention not supported!");
    //n_assert2(N_GL_EXTENSION_SUPPORTED(WGL_ARB_render_texture), "nGLServer2::nGLServer2(): WGL_ARB_render_texture extention not supported!");
}

//------------------------------------------------------------------------------
/**
*/
nGLServer2::~nGLServer2()
{
    // shut down all
    if (this->displayOpen)
    {
        this->CloseDisplay();
    }

    this->windowHandler.CloseWindow();
    //n_assert(this->textNodeList.IsEmpty());

    n_assert(0 != Singleton);
    Singleton = 0;
}

//-----------------------------------------------------------------------------
/**
    Open the display, show the window
*/
bool
nGLServer2::OpenDisplay()
{
    n_assert(!this->displayOpen);
    if (!this->windowHandler.IsWindowOpen())
    {
        // lazy initialization: open the app window
        // don't do this in the constructor because the window's name and icon
        // won't have been set at that time.
        this->windowHandler.OpenWindow();
    }

    if (this->DeviceOpen())
    {
        nGfxServer2::OpenDisplay();

        // clear display
        if (this->BeginFrame())
        {
            if (this->BeginScene())
            {
                this->Clear(AllBuffers, 0.0f, 0.0f, 0.0f, 1.0f, 1.0, 0);
                this->EndScene();
                this->PresentScene();
            }
            this->EndFrame();
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Close the display
*/
void
nGLServer2::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->DeviceClose();
    nGfxServer2::CloseDisplay();
}

//-----------------------------------------------------------------------------
/**
    Implements the Windows message pump. Must be called once a
    frame OUTSIDE of BeginScene() / EndScene().

    @return     false if nGLServer2 requests to shutdown the application
*/
bool
nGLServer2::Trigger()
{
    return this->windowHandler.Trigger();
}

//-----------------------------------------------------------------------------
/**
    Create screen shot and save it to given filename. (.jpg file)

    @param  fileName    filename for screen shot.
*/
bool
nGLServer2::SaveScreenshot(const char* fileName, nTexture2::FileFormat fileFormat)
{/*
    n_assert(fileName);
    n_assert(this->d3d9Device);
    HRESULT hr;

    // get mangled path name
    nString mangledPath = nFileServer2::Instance()->ManglePath(fileName);

    // copy back buffer to offscreen surface
    hr = this->d3d9Device->GetRenderTargetData(this->backBufferSurface, this->captureSurface);
    n_assert(SUCCEEDED(hr));

    // save image
    D3DXIMAGE_FILEFORMAT d3dxFormat = nD3D9Texture::FileFormatToD3DX(fileFormat);
    hr = D3DXSaveSurfaceToFile(mangledPath.Get(), d3dxFormat, this->captureSurface, NULL, NULL);
    n_assert(SUCCEEDED(hr));

    // all ok
    return true;
*/
    return false;
}

//-----------------------------------------------------------------------------
/**
    Enter dialog box mode.
*/
void
nGLServer2::EnterDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->hDC);

    nGfxServer2::EnterDialogBoxMode();

    // reset the device with lockable backbuffer flag
    this->OnDeviceCleanup(false);
    //D3DPRESENT_PARAMETERS p = this->presentParams;
    //p.MultiSampleType = D3DMULTISAMPLE_NONE;
    //p.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    //hr = this->d3d9Device->Reset(&p);
    this->InitDeviceState();
    this->OnDeviceInit(false);

    //hr = this->d3d9Device->SetDialogBoxMode(TRUE);
}

//-----------------------------------------------------------------------------
/**
    Leave dialog box mode.
*/
void
nGLServer2::LeaveDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->hDC);
    nGfxServer2::LeaveDialogBoxMode();

    //HRESULT hr = this->d3d9Device->SetDialogBoxMode(FALSE);

    //// only reset the device if it is currently valid
    //hr = this->d3d9Device->TestCooperativeLevel();
    //if (SUCCEEDED(hr))
    //{
    //    this->OnDeviceCleanup(false);
    //    hr = this->d3d9Device->Reset(&this->presentParams);
    //    this->InitDeviceState();
    //    this->OnDeviceInit(false);
    //}
}

//-----------------------------------------------------------------------------
/**
    Set the current display mode. This will not take effect until
    OpenDisplay() has been called!
*/
void
nGLServer2::SetDisplayMode(const nDisplayMode2& mode)
{
    this->windowHandler.SetDisplayMode(mode);
}

//-----------------------------------------------------------------------------
/**
    Get the current display mode.
*/
const nDisplayMode2&
nGLServer2::GetDisplayMode() const
{
    return this->windowHandler.GetDisplayMode();
}

//-----------------------------------------------------------------------------
/**
*/
void
nGLServer2::SetWindowTitle(const char* title)
{
    this->windowHandler.SetWindowTitle(title);
}
