//------------------------------------------------------------------------------
//  nglserver2_device.cc
//  2006 - haron
//------------------------------------------------------------------------------
#include "opengl/nglserver2.h"
#include "kernel/ntimeserver.h"
#include "resource/nresourceserver.h"

#include "opengl/nglextensionserver.h"
#include "opengl/nglmesh.h"
#include "opengl/ngltexture.h"

//------------------------------------------------------------------------------
/**
    Initialize the default state of the device. Must be called after
    creating or resetting the gl device.
*/
void
nGLServer2::InitDeviceState()
{
    n_assert(this->context);

    // update the viewport rectangle
    //D3DVIEWPORT9 dvp;
    //this->d3d9Device->GetViewport(&dvp);
    //this->viewport.x      = (float) dvp.X;
    //this->viewport.y      = (float) dvp.Y;
    //this->viewport.width  = (float) dvp.Width;
    //this->viewport.height = (float) dvp.Height;
    //this->viewport.nearz  = dvp.MinZ;
    //this->viewport.farz   = dvp.MaxZ;

    // update the projection matrix
    this->SetCamera(this->GetCamera());

    // set initial renderstates
    glFrontFace(GL_CW);
    glEnable(GL_DITHER);
    glDisable(GL_LIGHTING);

    glCullFace(GL_FRONT_AND_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //GL_FILL

    n_gltrace("nGLServer2::InitDeviceState().");
}

//------------------------------------------------------------------------------
/**
    Update the feature set member.
*/
void
nGLServer2::UpdateFeatureSet()
{
/*
    if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(2, 0))
    {
        this->featureSet = DX9;
        n_printf("nD3D9Server: DX9 feature set implemented.\n");

        // check if floating point textures are available as render target
        HRESULT hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                                   N_D3D9_DEVICETYPE,
                                                   D3DFMT_X8R8G8B8,
                                                   D3DUSAGE_RENDERTARGET,
                                                   D3DRTYPE_TEXTURE,
                                                   D3DFMT_R32F);
        if (SUCCEEDED(hr))
        {
            this->featureSet = DX9;
            n_printf("nD3D9Server: DX9 feature set implemented.\n");
        }
        else
  
        {
            this->featureSet = DX7;
            n_printf("nD3D9Server: DX7 feature set implemented.\n");
        }
    }
    else if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
    {
        // check if depth buffers are supported as textures
        HRESULT hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                                   N_D3D9_DEVICETYPE,
                                                   D3DFMT_X8R8G8B8,
                                                   0,
                                                   D3DRTYPE_TEXTURE,
                                                   D3DFMT_D24S8);
        if (SUCCEEDED(hr))
        {
            this->featureSet = DX8SB;
            n_printf("nD3D9Server: DX8SB feature set available.\n");
        }
        else
        {
            this->featureSet = DX8;
            n_printf("nD3D9Server: DX8 feature set available.\n");
        }
    }
    else
    {
        this->featureSet = DX7;
        n_printf("nD3D9Server: DX7 feature set implemented.\n");
    }
        */
    //n_printf("nGLServer2: OpenGL feature set implemented.\n");
}

//------------------------------------------------------------------------------
/**
    Open and initialize the gl device, make the app window visible.
*/
bool
nGLServer2::DeviceOpen()
{
    n_assert(!this->hDC);
    //n_assert(this->context);
    //n_assert(this->windowOpen);
    //n_assert(this->windowMinimized);
    n_assert(this->windowHandler.IsWindowOpen());
    n_assert(this->windowHandler.IsWindowMinimized());
    n_assert(!this->inBeginScene);
/*
    #ifdef __NEBULA_STATS__
    n_assert(0 == this->queryResourceManager);
    #endif
*/

    n_printf("nGLServer2::DeviceOpen()\n");

    // prepare window...
    this->windowHandler.AdjustWindowForChange();

    this->hDC = GetDC(this->windowHandler.GetHwnd());

    // init open opengl context
    this->ContextOpen();

    // create shader device
    //bool res = nCgFXShader::CreateDevice();

    //display info
    n_printf("GL - Extensions\n");
    n_printf("    Vendor:   %s\n", glGetString(GL_VENDOR));
    n_printf("    Renderer: %s\n", glGetString(GL_RENDERER));
    n_printf("    Version:  %s\n", glGetString(GL_VERSION));
    n_printf("Supported Extensions:\n");
    nGLExtensionServer::PrintExtensions(nString((const char*)glGetString(GL_EXTENSIONS)));
    
    //init extensitions
    //nGLExtensionServer::InitExtensions();

    // reload any resources if necessary
    this->OnDeviceInit(true);

    // initialize gl device state
    this->InitDeviceState();

    // restore window
    this->windowHandler.RestoreWindow();
    
    n_gltrace("nGLServer2::DeviceOpen().");
    return true; //res;
}

//------------------------------------------------------------------------------
/**
    Close the gl device, minimize the app window.
*/
void
nGLServer2::DeviceClose()
{
    n_assert(this->hDC);
    n_assert(this->windowHandler.IsWindowOpen());
    n_assert(!this->windowHandler.IsWindowMinimized());
    n_assert(this->windowHandler.GetHwnd());

    n_printf("nGLServer2::DeviceClose()\n");

    // unload all resources
    this->OnDeviceCleanup(true);

    // destroy shader device
    //nCgFXShader::ReleaseDevice();

    // close gl context
    this->ContextClose();

    // unload all resources
    this->OnDeviceCleanup(true);

    // destroy gl device
    ReleaseDC(this->windowHandler.GetHwnd(), this->hDC);
    this->hDC = NULL;
    
    // minimze the app window
    this->windowHandler.MinimizeWindow();
    n_gltrace("nGLServer2::DeviceClose().");
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
        this->OnDeviceCleanup(false);

        // if we are in windowed mode, the cause for the reset may be a display
        // mode change of the desktop, in this case we need to find new
        // buffer pixel formats
        //if (this->displayMode.GetType() == nDisplayMode2::Windowed)
        //{
            D3DFORMAT dispFormat;
            D3DFORMAT backFormat;
            D3DFORMAT zbufFormat;
            this->FindBufferFormats(this->GetDisplayMode().GetBpp(), dispFormat, backFormat, zbufFormat);
            this->presentParams.BackBufferFormat       = backFormat;
            this->presentParams.AutoDepthStencilFormat = zbufFormat;
        //}

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

        // reload the resource
        this->OnDeviceInit(false);

        // initialize the device
        this->InitDeviceState();

        return true;
    }
    else
    {
        // device cannot be restored at this time
        return false;
    }
*/
    return false;
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
    nTime curTime = kernelServer->GetTimeServer()->GetTime();
    nTime diff = curTime - this->timeStamp;
    if (diff <= 0.000001f)
    {
        diff = 0.000001f;
    }
    WATCHER_SET_FLOAT(watchFPS, float(1.0f / diff));
    WATCHER_SET_INT(watchNumPrimitives, this->statsNumPrimitives);
    WATCHER_SET_INT(watchNumDrawCalls, this->statsNumDrawCalls);
    WATCHER_SET_INT(watchNumRenderStateChanges, this->statsNumRenderStateChanges);

    this->timeStamp = curTime;
    this->statsNumRenderStateChanges = 0;
    this->statsNumTextureChanges = 0;
    this->statsNumDrawCalls = 0;
    this->statsNumPrimitives = 0;

    // no resource manager query if not running the debug runtime
/*
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

//------------------------------------------------------------------------------
/**
    Updates the mouse cursor image and visibility. Should be called
    once per frame.
*/
void
nGLServer2::UpdateCursor()
{
    if (this->cursorDirty)
    {
        this->cursorDirty = false;
/*
        nTexture2* tex = this->curMouseCursor.GetTexture();
        if (tex)
        {
            if (!tex->IsValid())
            {
                bool mouseCursorLoaded = tex->Load();
                n_assert(mouseCursorLoaded);
                this->cursorDirty = true;
            }

            HRESULT hr;
            IDirect3DTexture9* d3d9Tex = ((nD3D9Texture*)tex)->GetTexture2D();
            n_assert(d3d9Tex);

            IDirect3DSurface9* surfPtr = 0;
            hr = d3d9Tex->GetSurfaceLevel(0, &surfPtr);
            n_assert(SUCCEEDED(hr));
            n_assert(surfPtr);

            int hotspotX = this->curMouseCursor.GetHotspotX();
            int hotspotY = this->curMouseCursor.GetHotspotY();
            hr = this->d3d9Device->SetCursorProperties(hotspotX, hotspotY, surfPtr);
            n_assert(SUCCEEDED(hr));

            switch (this->cursorVisibility)
            {
                case nGfxServer2::None:
                    SetCursor(NULL);
                    this->d3d9Device->ShowCursor(FALSE);
                    break;

                case nGfxServer2::System:
                    this->d3d9Device->ShowCursor(FALSE);
                    break;

                case nGfxServer2::Custom:
                    SetCursor(NULL);
                    this->d3d9Device->ShowCursor(TRUE);
                    break;
            }
        }*/
    }
    // NOTE: cursor visibility is handled inside WinProc!
}
