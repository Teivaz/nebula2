#define N_IMPLEMENT nD3D9Server
//------------------------------------------------------------------------------
//  nd3d9server_device.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"

//------------------------------------------------------------------------------
/**
    Initialize the default state of the device. Must be called after
    creating or resetting the d3d device.
*/
void
nD3D9Server::InitDeviceState()
{
    n_assert(this->d3d9Device);

    // update the projection matrix
    this->SetCamera(this->GetCamera());

    // set initial renderstates
    this->d3d9Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    this->d3d9Device->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    this->d3d9Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    // update the device caps
    this->d3d9Device->GetDeviceCaps(&(this->devCaps));
}

//------------------------------------------------------------------------------
/**
    Helper method to verify a given pixelformat combination for the frame
    and depth buffer.
*/
bool
nD3D9Server::CheckDepthFormat(D3DFORMAT adapterFormat, 
                              D3DFORMAT backbufferFormat,
                              D3DFORMAT depthFormat) 
{
    n_assert(this->d3d9);

    HRESULT hr;

    // verify that the depth format exists
    hr = this->d3d9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
                                       N_D3D9_DEVICETYPE,
                                       adapterFormat,
                                       D3DUSAGE_DEPTHSTENCIL,
                                       D3DRTYPE_SURFACE,
                                       depthFormat);
    if (FAILED(hr))
    {
        return false;
    }

    // check if compatible with display mode
    hr = this->d3d9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
                                            N_D3D9_DEVICETYPE,
                                            adapterFormat,
                                            backbufferFormat,
                                            depthFormat);
    return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
    Find the best possible combination of buffer formats. The method will
    fail hard if no matching buffer formats could be found!
*/
void
nD3D9Server::FindBufferFormats(D3DFORMAT& dispFormat, D3DFORMAT& backFormat, D3DFORMAT& zbufFormat)
{
    HRESULT hr;

    // table of valid pixel format combinations
    // { adapterFormat, backbufferFormat, zbufferFormat }
    D3DFORMAT formatTable[][3] = {
        { D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_D24S8 },
        { D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_D24X4S4 },
        { D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_D24X8 },
        { D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_D16 },
        { D3DFMT_UNKNOWN,  D3DFMT_UNKNOWN,  D3DFMT_UNKNOWN },
    };
    if (this->displayMode.GetType() == nDisplayMode2::FULLSCREEN)
    {
        // find fullscreen buffer formats
        dispFormat = D3DFMT_UNKNOWN;
        backFormat = D3DFMT_UNKNOWN;
        zbufFormat = D3DFMT_UNKNOWN;
        int i;
        for (i = 0; formatTable[i][0] != D3DFMT_UNKNOWN; i++)
        {
            dispFormat = formatTable[i][0];
            backFormat = formatTable[i][1];
            zbufFormat = formatTable[i][2];
            if (this->CheckDepthFormat(dispFormat, backFormat, zbufFormat))
            {
                break;
            }
        }
    }
    else
    {
        // find windowed mode buffer format, the display and backbuffer formats
        // are defined by the current desktop color depth
        D3DDISPLAYMODE desktopMode;
        hr = this->d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &desktopMode);
        n_assert(SUCCEEDED(hr));

        dispFormat = desktopMode.Format;
        switch (dispFormat)
        {
            case D3DFMT_R8G8B8:
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
                backFormat = D3DFMT_A8R8G8B8;
                break;

            default:
                backFormat = dispFormat;
                break;
        }
        int i;
        for (i = 0; formatTable[i][0] != D3DFMT_UNKNOWN; i++)
        {
            zbufFormat = formatTable[i][2];
            if (this->CheckDepthFormat(dispFormat, backFormat, zbufFormat))
            {
                break;
            }
        }
    }

    if (D3DFMT_UNKNOWN == zbufFormat)
    {
        n_error("nD3D9Server: No valid Direct3D display format found!\n");
    }
}    

//------------------------------------------------------------------------------
/**
    Open and initialize the d3d device, make the app window visible.
*/
bool
nD3D9Server::DeviceOpen()
{
    n_assert(this->hWnd);
    n_assert(this->d3d9);
    n_assert(!this->d3d9Device);
    n_assert(this->windowOpen);
    n_assert(this->windowMinimized);
    n_assert(!this->inBeginScene);
    n_assert(0 == this->depthStencilSurface);
    n_assert(0 == this->backBufferSurface);
    
    HRESULT hr;

    // prepare window...
    this->AdjustWindowForChange();

    // find a valid combination of buffer formats
    D3DFORMAT dispFormat;
    D3DFORMAT backFormat;
    D3DFORMAT zbufFormat;
    this->FindBufferFormats(dispFormat, backFormat, zbufFormat);

    // get d3d device caps for HW T&L decision
    hr = this->d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, N_D3D9_DEVICETYPE, &(this->devCaps));
    n_assert(SUCCEEDED(hr));

    // define device behaviour flags
    this->deviceBehaviourFlags = D3DCREATE_FPU_PRESERVE;

    #if N_D3D9_DEBUG

        this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        this->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        n_printf("nD3D9Server: using enforced software vertex processing\n");

    #else 
        // check if hardware vertex shaders are supported, if not,
        // activate software vertex processing
        if (this->devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            if (this->devCaps.VertexShaderVersion >= D3DVS_VERSION(1, 0))
            {        
                this->deviceBehaviourFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
                n_printf("nD3D9Server: using hardware vertex processing\n");
                if (this->devCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
                {
                    this->deviceBehaviourFlags |= D3DCREATE_PUREDEVICE;
                }
            }
            else
            {
                this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                n_printf("nD3D9Server: using software vertex processing\n");
            }
        }
        else
        {
            this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            n_printf("nD3D9Server: using software vertex processing\n");
        }
        this->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    #endif


    // define presentation parameters
    if (this->displayMode.GetType() == nDisplayMode2::FULLSCREEN)
    {
        this->presentParams.BackBufferCount  = 1;
        this->presentParams.Windowed         = FALSE;
    }
    else
    {
        this->presentParams.BackBufferCount  = 1;
        this->presentParams.Windowed         = TRUE;
    }

    if (this->displayMode.GetType() == nDisplayMode2::CHILDWINDOWED) 
    {
        // if parent window exist, get parent dimension
        RECT r;
        GetClientRect(this->parentHWnd, &r);
        this->displayMode.SetWidth((ushort)(r.right - r.left));
        this->displayMode.SetHeight((ushort)(r.bottom - r.top));
    }

    if (this->displayMode.GetVerticalSync())
    {
        this->presentParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }
    else
    {
        this->presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    this->presentParams.BackBufferWidth                 = this->displayMode.GetWidth();
    this->presentParams.BackBufferHeight                = this->displayMode.GetHeight();
    this->presentParams.BackBufferFormat                = backFormat;
    this->presentParams.MultiSampleType                 = D3DMULTISAMPLE_NONE;
    this->presentParams.MultiSampleQuality              = 0;
    this->presentParams.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    this->presentParams.hDeviceWindow                   = this->hWnd;
    this->presentParams.EnableAutoDepthStencil          = TRUE;
    this->presentParams.AutoDepthStencilFormat          = zbufFormat;
    this->presentParams.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;

    // create d3d device
    hr = this->d3d9->CreateDevice(D3DADAPTER_DEFAULT,
                                  N_D3D9_DEVICETYPE,
                                  this->hWnd,
                                  this->deviceBehaviourFlags,
                                  &(this->presentParams),
                                  &(this->d3d9Device));
    if (FAILED(hr))
    {
        n_error("nD3D9Server: Could not create d3d device!\n");
        return false;
    }
    n_assert(this->d3d9Device);

    // initialize d3d device state
    this->InitDeviceState();

    // reload any resources if necessary
    this->ReloadResources();

    // restore window
    this->RestoreWindow();
    
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the d3d device, minimize the app window.
*/
void
nD3D9Server::DeviceClose()
{
    n_assert(this->d3d9);
    n_assert(this->d3d9Device);
    n_assert(this->windowOpen);
    n_assert(!this->windowMinimized);
    n_assert(this->hWnd);

    // unload all resources
    this->UnloadResources();

    // release refs on original backbuffer and depth/stencil surfaces
    if (this->backBufferSurface)
    {
        this->backBufferSurface->Release();
        this->backBufferSurface = 0;
    }
    if (this->depthStencilSurface)
    {
        this->depthStencilSurface->Release();
        this->depthStencilSurface = 0;
    }

    // destroy d3d device
    this->d3d9Device->Release();
    this->d3d9Device = 0;
    
    // minimze the app window
    this->MinimizeWindow();
}

//------------------------------------------------------------------------------
/**
    Check for lost device, and try to reset the device.
*/
bool
nD3D9Server::TestResetDevice()
{
    n_assert(this->d3d9Device);
    HRESULT hr;

    hr = this->d3d9Device->TestCooperativeLevel();
    if (SUCCEEDED(hr))
    {
        return true;
    }
    else if (D3DERR_DEVICENOTRESET == hr)
    {
        // device is ready to be reset, invole the reanimation procedure...
        this->UnloadResources();

        // if we are in windowed mode, the cause for the reset may be a display
        // mode change of the desktop, in this case we need to find new
        // buffer pixel formats
        if (this->displayMode.GetType() == nDisplayMode2::WINDOWED)
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
        if (FAILED(hr))
        {
            n_printf("nD3D9Server: Failed to reset d3d device!\n");
            return false;
        }

        n_printf("nD3D9Server: Device reset!\n");

        // initialize the device
        this->InitDeviceState();

        // reload the resource
        this->ReloadResources();

        return true;
    }
    else
    {
        // device cannot be restored at this time
        n_sleep(0.1);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Returns true if device is a software vertex processing device.
    This is a private method called by nD3D9Mesh to decide whether
    vertex buffer must be created in software vertex processing mode.
*/
bool
nD3D9Server::GetSoftwareVertexProcessing() const
{
    return 0 != (this->deviceBehaviourFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING);
}
