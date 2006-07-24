//------------------------------------------------------------------------------
//  nglserver2_main.cc
//  2003-cubejk
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"

#include "opengl/nglmesh.h"
#include "opengl/ngltexture.h"
#include "opengl/nglextensionserver.h"

#include "kernel/ntimeserver.h"

nNebulaClass(nGLServer2, "ngfxserver2");

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
        n_error(message.Get());
    }
}

//------------------------------------------------------------------------------
/**
*/
nGLServer2::nGLServer2() :
    #ifdef __NEBULA_STATS__
    timeStamp(0.0),
    //queryResourceManager(0),
    dbgQueryTextureTrashing("gfxTexTrashing", nArg::Bool),
    dbgQueryTextureApproxBytesDownloaded("gfxTexApproxBytesDownloaded", nArg::Int),
    dbgQueryTextureNumEvicts("gfxTexNumEvicts", nArg::Int),
    dbgQueryTextureNumVidCreates("gfxTexNumVidCreates", nArg::Int),
    dbgQueryTextureLastPri("gfxTexLastPri", nArg::Int),
    dbgQueryTextureNumUsed("gfxTexNumUsed", nArg::Int),
    dbgQueryTextureNumUsedInVidMem("gfxTexNumUsedInVidMem", nArg::Int),
    dbgQueryTextureWorkingSet("gfxTexWorkingSet", nArg::Int),
    dbgQueryTextureWorkingSetBytes("gfxTexWorkingSetBytes", nArg::Int),
    dbgQueryTextureTotalManaged("gfxTexTotalManaged", nArg::Int),
    dbgQueryTextureTotalBytes("gfxTexTotalBytes", nArg::Int),
    dbgQueryNumPrimitives("gfxNumPrimitives", nArg::Int),
    dbgQueryFPS("gfxFPS", nArg::Float),
    dbgQueryNumDrawCalls("gfxNumDrawCalls", nArg::Int),
    dbgQueryNumRenderStateChanges("gfxNumRenderStateChanges", nArg::Int),
    dbgQueryNumTextureChanges("gfxNumTextureChanges", nArg::Int),
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
    // open the app window
    this->windowHandler.OpenWindow();

    nKernelServer::Instance()->New("nglextensionserver", "/sys/servers/glextension");
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
        this->windowHandler.OpenWindow();
    }

    if (this->DeviceOpen())
    {
        nGfxServer2::OpenDisplay();

        // clear display
        if (this->BeginScene())
        {
            this->Clear(AllBuffers, 0.0f, 0.0f, 0.0f, 1.0f, 1.0, 0);
            this->EndScene();
            this->PresentScene();
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

    @return     false if the system requests to shutdown the application
*/
bool
nGLServer2::Trigger()
{
    return this->windowHandler.Trigger();
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
    //HRESULT hr = this->d3d9Device->SetDialogBoxMode(TRUE);
    //n_assert(SUCCEEDED(hr));
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
    //n_assert(SUCCEEDED(hr));
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

//------------------------------------------------------------------------------
/**
    Check for lost device, and try to reset the device.
*/
bool
nGLServer2::TestResetDevice()
{
    n_assert(this->hDC);
/*    HRESULT hr;

    hr = this->d3d9Device->TestCooperativeLevel();
    if (SUCCEEDED(hr))
    {
        return true;
    }
    else if (D3DERR_DEVICENOTRESET == hr)
    {
        // device is ready to be reset, invoke the reanimation procedure...
        this->OnDeviceLost();

        // if we are in windowed mode, the cause for the reset may be a display
        // mode change of the desktop, in this case we need to find new
        // buffer pixel formats
        if (this->displayMode.GetType() == nDisplayMode2::Windowed)
        {
            D3DFORMAT dispFormat;
            D3DFORMAT backFormat;
            D3DFORMAT zbufFormat;
            this->FindBufferFormats(dispFormat, backFormat, zbufFormat);

            this->presentParams.BackBufferFormat       = backFormat;
            this->presentParams.AutoDepthStencilFormat = zbufFormat;
        }

        // try to reset the device
        hr = this->d3d9Device->Reset(&this->presentParams);
        if (D3DERR_INVALIDCALL == hr)
        {
            n_assert(false);
        }
        if (FAILED(hr))
        {
            n_printf("nD3D9Server: Failed to reset d3d device!\n");
            return false;
        }

        n_printf("nD3D9Server: Device reset!\n");

        // initialize the device
        this->InitDeviceState();

        // reload the resource
        this->OnRestoreDevice();

        return true;
    }
    else*/
    {
        // device cannot be restored at this time
        n_sleep(0.1);
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Get the supported feature set.
*/
nGfxServer2::FeatureSet
nGLServer2::GetFeatureSet()
{
    if (InvalidFeatureSet != this->featureSetOverride)
    {
        return this->featureSetOverride;
    }
    else
    {
        return this->featureSet;
    }
}

#ifdef __NEBULA_STATS__
//------------------------------------------------------------------------------
/**
    Query the resource manager and fill the watcher variables with the
    result.
*/
void
nGLServer2::QueryStatistics()
{
    // compute frames per second
    nTime curTime = nTimeServer::Instance()->GetTime();
    nTime diff = curTime - this->timeStamp;
    if (diff <= 0.000001f)
    {
        diff = 0.000001f;
    }
    this->dbgQueryFPS->SetF(float(1.0 / diff));
    this->timeStamp = curTime;

    this->dbgQueryNumRenderStateChanges->SetI(this->statsNumRenderStateChanges);
    this->dbgQueryNumTextureChanges->SetI(this->statsNumTextureChanges);
/*
    // no resource manager query if not running the debug runtime
    if (0 == this->queryResourceManager)
    {
        return;
    }

    HRESULT hr = this->queryResourceManager->Issue(D3DISSUE_END);
    if (SUCCEEDED(hr))
    {
        D3DDEVINFO_RESOURCEMANAGER result;
        hr = this->queryResourceManager->GetData(&result, sizeof(result), 0);
        if (SUCCEEDED(hr))
        {
            D3DRESOURCESTATS stats;

            // sum texture and cube texture stats
            memcpy(&stats, &(result.stats[D3DRTYPE_TEXTURE]), sizeof(stats));
            stats.bThrashing            |= result.stats[D3DRTYPE_CUBETEXTURE].bThrashing;
            stats.ApproxBytesDownloaded += result.stats[D3DRTYPE_CUBETEXTURE].ApproxBytesDownloaded;
            stats.NumEvicts             += result.stats[D3DRTYPE_CUBETEXTURE].NumEvicts;
            stats.NumVidCreates         += result.stats[D3DRTYPE_CUBETEXTURE].NumVidCreates;
            stats.LastPri               += result.stats[D3DRTYPE_CUBETEXTURE].LastPri;
            stats.NumUsed               += result.stats[D3DRTYPE_CUBETEXTURE].NumUsed;
            stats.NumUsedInVidMem       += result.stats[D3DRTYPE_CUBETEXTURE].NumUsedInVidMem;
            stats.WorkingSet            += result.stats[D3DRTYPE_CUBETEXTURE].WorkingSet;
            stats.WorkingSetBytes       += result.stats[D3DRTYPE_CUBETEXTURE].WorkingSetBytes;
            stats.TotalManaged          += result.stats[D3DRTYPE_CUBETEXTURE].TotalManaged;
            stats.TotalBytes            += result.stats[D3DRTYPE_CUBETEXTURE].TotalBytes;

            // fill watcher variables
            this->dbgQueryTextureTrashing->SetB(stats.bThrashing ? true : false);
            this->dbgQueryTextureApproxBytesDownloaded->SetI(stats.ApproxBytesDownloaded);
            this->dbgQueryTextureNumEvicts->SetI(stats.NumEvicts);
            this->dbgQueryTextureNumVidCreates->SetI(stats.NumVidCreates);
            this->dbgQueryTextureLastPri->SetI(stats.LastPri);
            this->dbgQueryTextureNumUsed->SetI(stats.NumUsed);
            this->dbgQueryTextureNumUsedInVidMem->SetI(stats.NumUsedInVidMem);
            this->dbgQueryTextureWorkingSet->SetI(stats.WorkingSet);
            this->dbgQueryTextureWorkingSetBytes->SetI(stats.WorkingSetBytes);
            this->dbgQueryTextureTotalManaged->SetI(stats.TotalManaged);
            this->dbgQueryTextureTotalBytes->SetI(stats.TotalBytes);
        }
    }
*/
}
#endif
