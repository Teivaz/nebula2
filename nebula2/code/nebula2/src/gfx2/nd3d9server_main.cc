//------------------------------------------------------------------------------
//  nd3d9server_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9server.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nD3D9Server, "ngfxserver2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Server::nD3D9Server() :
    #ifdef __NEBULA_STATS__
    timeStamp(0.0),
    queryResourceManager(0),
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
    deviceBehaviourFlags(0),
    d3dSprite(0),
    d3d9(0),
    d3d9Device(0),
    depthStencilSurface(0),
    backBufferSurface(0),
    effectPool(0),
    featureSet(InvalidFeatureSet),
    d3dxLine(0),
    winVersion(UnknownWin)
{
    memset(&(this->devCaps), 0, sizeof(this->devCaps));
    memset(&(this->presentParams), 0, sizeof(this->presentParams));
    memset(&(this->shapeMeshes), 0, sizeof(this->shapeMeshes));

    // detect windows version
    this->DetectWindowsVersion();

    // initialize Direct3D
    this->D3dOpen();
}
  
//------------------------------------------------------------------------------
/**
*/
nD3D9Server::~nD3D9Server()
{
    // shut down all
    if (this->displayOpen)
    {
        this->CloseDisplay();
    }
    this->D3dClose();
    this->windowHandler.CloseWindow();
    n_assert(this->textNodeList.IsEmpty());
}

//------------------------------------------------------------------------------
/**
    Initialize Direct3D.
*/
void
nD3D9Server::D3dOpen()
{
    n_assert(0 == this->d3d9);

    // create the d3d object
    this->d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!this->d3d9)
    {
        n_error("nD3D9Server: could not initialize Direct3D!\n");
    }

    // update the feature set
    this->UpdateFeatureSet();
}

//------------------------------------------------------------------------------
/**
    Shutdown Direct3D.
*/
void
nD3D9Server::D3dClose()
{
    n_assert(this->d3d9);
    n_assert(0 == this->d3d9Device);

    // release the d3d object
    int refCount = this->d3d9->Release();
    if (0 < refCount) 
    {
        n_printf("WARNING: Direct3D9 interface was still referenced (count = %d)\n", refCount);
    }
    this->d3d9 = 0;
}

//-----------------------------------------------------------------------------
/**
    Open the display.
*/
bool
nD3D9Server::OpenDisplay()
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
    Close the display.
*/
void
nD3D9Server::CloseDisplay()
{
    n_assert(this->displayOpen);
    this->DeviceClose();
    nGfxServer2::CloseDisplay();
}

//-----------------------------------------------------------------------------
/**
    Implements the Windows message pump. Must be called once a
    frame OUTSIDE of BeginScene() / EndScene().

    @return     false if nD3D9Server requests to shutdown the application
*/
bool
nD3D9Server::Trigger()
{
    return this->windowHandler.Trigger();
}

//-----------------------------------------------------------------------------
/**
    Create screen shot and save it to given filename. (.bmp file)

    @param fileName filename for screen shot.
*/
bool
nD3D9Server::SaveScreenshot(const char* fileName)
{
    n_assert(fileName);
    HRESULT hr;

    // get adapter number and device window
    D3DDEVICE_CREATION_PARAMETERS dcp;
    this->d3d9Device->GetCreationParameters(&dcp);

    // get width and height for the front buffer surface
    D3DDISPLAYMODE dispMode;
    hr = this->d3d9->GetAdapterDisplayMode(dcp.AdapterOrdinal, &dispMode);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to get 'adapter display mode'!\n");
        return false;
    }

    // create the front buffer surface to save for screenshot. 
    IDirect3DSurface9* surf;
    hr = this->d3d9Device->CreateOffscreenPlainSurface(dispMode.Width, dispMode.Height, 
                                                       D3DFMT_A8R8G8B8,
                                                       D3DPOOL_SCRATCH,
                                                       &surf,
                                                       NULL);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to 'create offscreen plain surface'!\n");
        return false;
    }

    // get a copy of the front buffer surface.
    this->d3d9Device->GetFrontBufferData(0, surf);

    // get the rectangle into which rendering is drawn
    // it's the client rectangle of the focus window in screen coordinates
    RECT rc;
    GetClientRect(dcp.hFocusWindow, &rc);
    ClientToScreen(dcp.hFocusWindow, LPPOINT(&rc.left));
    ClientToScreen(dcp.hFocusWindow, LPPOINT(&rc.right));

    // mangle filename
    nString mangledPath = nFileServer2::Instance()->ManglePath(fileName);

    // save the front buffer surface to given filename.
    hr = D3DXSaveSurfaceToFile(mangledPath.Get(), D3DXIFF_BMP, surf, 0, &rc);
    if (FAILED(hr))
    {
        n_printf("nD3D9Server::Screenshot(): Failed to save file '%s'!\n", fileName);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Enter dialog box mode.
*/
void
nD3D9Server::EnterDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->d3d9Device);
    nGfxServer2::EnterDialogBoxMode();
    HRESULT hr = this->d3d9Device->SetDialogBoxMode(TRUE);
    n_dxtrace(hr, "nD3D9Server::EnterDialogBoxMode()");

    // reset the device to fix a know issue for win98/ME where this dialogbox is sometimes hidden
    if (Win32_Windows == this->GetWindowsVersion()) // windows 95 and family
    {
        // invoke the reanimation procedure...
        this->OnDeviceLost(false);

        // if we are in windowed mode, the cause for the reset may be a display
        // mode change of the desktop, in this case we need to find new
        // buffer pixel formats
        D3DFORMAT dispFormat;
        D3DFORMAT backFormat;
        D3DFORMAT zbufFormat;
        this->FindBufferFormats(this->GetDisplayMode().GetBpp(), dispFormat, backFormat, zbufFormat);
        this->presentParams.BackBufferFormat       = backFormat;
        this->presentParams.AutoDepthStencilFormat = zbufFormat;

        hr = this->d3d9Device->Reset(&this->presentParams);
        if (D3DERR_INVALIDCALL == hr ||FAILED(hr))
        {
            n_error("nD3D9Server: Failed to reset d3d device!\n");
        }
        // initialize the device
        this->InitDeviceState();

        // reload the resource
        this->OnRestoreDevice();
    }
}

//-----------------------------------------------------------------------------
/**
    Leave dialog box mode.
*/
void
nD3D9Server::LeaveDialogBoxMode()
{
    n_assert(this->windowHandler.GetDisplayMode().GetDialogBoxMode());
    n_assert(this->d3d9Device);
    nGfxServer2::LeaveDialogBoxMode();
    HRESULT hr = this->d3d9Device->SetDialogBoxMode(FALSE);
    n_dxtrace(hr, "nD3D9Server::LeaveDialogBoxMode()");
}

//-----------------------------------------------------------------------------
/**
    Set the current display mode. This will not take effect until
    OpenDisplay() has been called!
*/
void
nD3D9Server::SetDisplayMode(const nDisplayMode2& mode)
{
    this->windowHandler.SetDisplayMode(mode);
}

//-----------------------------------------------------------------------------
/**
    Get the current display mode.
*/
const nDisplayMode2&
nD3D9Server::GetDisplayMode() const
{
    return this->windowHandler.GetDisplayMode();
}

//-----------------------------------------------------------------------------
/**
    Detect the current Windows version,
    only makes a differnence betweem WinNT family, Win95 familiy and the rest.
*/
void
nD3D9Server::DetectWindowsVersion()
{
    // copy and paste from the windows platform SDK GetVersionEx example
    
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
        {
            this->winVersion = UnknownWin;
            return;
        }
    }

    switch (osvi.dwPlatformId)
    {
        case VER_PLATFORM_WIN32_NT:
        {
            this->winVersion = Win32_NT;
            /*
            // Test for the product.

            if ( osvi.dwMajorVersion <= 4 )
                printf("Microsoft Windows NT ");

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
                printf ("Microsoft Windows 2000 ");

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
                printf ("Microsoft Windows XP ");

            // Test for product type.

            if( bOsVersionInfoEx )
            {
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                {
                    if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                        printf ( "Personal " );
                    else
                        printf ( "Professional " );
                }
                else if ( osvi.wProductType == VER_NT_SERVER )
                {
                    if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        printf ( "DataCenter Server " );
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        printf ( "Advanced Server " );
                    else
                        printf ( "Server " );
                }
            }
            else
            {
                HKEY hKey;
                char szProductType[80];
                DWORD dwBufLen;

                RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                    "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
                    0, KEY_QUERY_VALUE, &hKey );
                RegQueryValueEx( hKey, "ProductType", NULL, NULL,
                    (LPBYTE) szProductType, &dwBufLen);
                RegCloseKey( hKey );
                if ( lstrcmpi( "WINNT", szProductType) == 0 )
                    printf( "Professional " );
                if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
                    printf( "Server " );
                if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
                    printf( "Advanced Server " );
            }

            // Display version, service pack (if any), and build number.

            if ( osvi.dwMajorVersion <= 4 )
            {
                printf ("version %d.%d %s (Build %d)\n",
                    osvi.dwMajorVersion,
                    osvi.dwMinorVersion,
                    osvi.szCSDVersion,
                    osvi.dwBuildNumber & 0xFFFF);
            }
            else
            { 
                printf ("%s (Build %d)\n",
                    osvi.szCSDVersion,
                    osvi.dwBuildNumber & 0xFFFF);
            }
            */
        }
        break;
        case VER_PLATFORM_WIN32_WINDOWS:
        {
            this->winVersion = Win32_Windows;
            /*
            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
            {
                printf ("Microsoft Windows 95 ");
                if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
                    printf("OSR2 " );
            } 

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
            {
                printf ("Microsoft Windows 98 ");
                if ( osvi.szCSDVersion[1] == 'A' )
                    printf("SE " );
            } 

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
            {
                printf ("Microsoft Windows Me ");
            } 
            */
        }
        break;
        case VER_PLATFORM_WIN32s:
        {
            this->winVersion = UnknownWin;
            //printf ("Microsoft Win32s ");
        }
        break;
   }
}
