//------------------------------------------------------------------------------
//  ndshowserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "video/ndshowserver.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nDShowServer, "nvideoserver");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    ndshowserver

    @cppclass
    nDShowServer
    
    @superclass
    nvideoserver
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nDShowServer::nDShowServer() :
    refGfxServer("/sys/servers/gfx"),
    refInputServer("/sys/servers/input"),
    refHwnd("/sys/env/hwnd"),
    graphBuilder(0),
    mediaControl(0),
    mediaEvent(0),
    videoWindow(0),
    firstFrame(false)
{
    HRESULT hr = CoInitialize(NULL);
}

//------------------------------------------------------------------------------
/**
*/
nDShowServer::~nDShowServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    CoUninitialize();
}

//------------------------------------------------------------------------------
/**
*/
bool
nDShowServer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(0 == this->graphBuilder);
    n_assert(0 == this->mediaControl);
    n_assert(0 == this->mediaEvent);
    n_assert(0 == this->videoWindow);
    nVideoServer::Open();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nDShowServer::Close()
{
    n_assert(this->IsOpen());
    if (this->IsPlaying())
    {
        this->Stop();
    }
    nVideoServer::Close();
}

//------------------------------------------------------------------------------
/**
*/
bool
nDShowServer::PlayFile(const char* filename)
{
    n_assert(filename);
    if (this->IsPlaying())
    {
        this->Stop();
    }
    n_assert(0 == this->graphBuilder);
    n_assert(0 == this->mediaControl);
    n_assert(0 == this->videoWindow);
    n_assert(0 == this->mediaEvent);

    HRESULT hr;
    nFileServer2* fileServer = kernelServer->GetFileServer();

    // clear background
    this->refGfxServer->BeginScene();
    this->refGfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0);
    this->refGfxServer->EndScene();
    this->refGfxServer->PresentScene();

    // mangle Nebula path into absolute path wide character string
    char mangledPath[N_MAXPATH];
    wchar_t widePath[N_MAXPATH];
    fileServer->ManglePath(filename, mangledPath, sizeof(mangledPath));
    mbstowcs(widePath, mangledPath, strlen(mangledPath) + 1);

    // create DirectShow filter graph
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&this->graphBuilder);
    if (FAILED(hr))
    {
        n_error("nDShowServer: could not create DirectShow filter graph!");
        return false;
    }

    // create the filter graph for the file
    hr = this->graphBuilder->RenderFile(widePath, NULL);
    if (FAILED(hr))
    {
        n_error("nDShowServer::PlayFile(): could not render file '%s'", mangledPath);
        return false;
    }

    // query required interfaces
    hr = this->graphBuilder->QueryInterface(IID_IMediaControl, (void**)&this->mediaControl);
    n_assert(SUCCEEDED(hr));
    hr = this->graphBuilder->QueryInterface(IID_IMediaEvent, (void**)&this->mediaEvent);
    n_assert(SUCCEEDED(hr));
    hr = this->graphBuilder->QueryInterface(IID_IVideoWindow, (void**) &this->videoWindow);
    n_assert(SUCCEEDED(hr));
    hr = this->graphBuilder->QueryInterface(IID_IBasicVideo, (void**) &this->basicVideo);

    // get the width and height of the video stream
    long videoWidth;
    long videoHeight;
    hr = this->basicVideo->GetVideoSize(&videoWidth, &videoHeight);
    n_assert(SUCCEEDED(hr));

    // get public window handle, which has been initialized by the gfx subsystem
    OAHWND ownerHwnd = (OAHWND) this->refHwnd->GetI();
    RECT rect;
    GetClientRect((HWND)ownerHwnd, &rect);
    LONG videoLeft = ((rect.right + rect.left) - videoWidth) / 2;
    LONG videoTop = ((rect.bottom + rect.top) - videoHeight) / 2;

    // enable dialog box mode in gfx server
    this->refGfxServer->EnterDialogBoxMode();

    // setup video window
    hr = this->videoWindow->put_AutoShow(OATRUE);
    n_assert(SUCCEEDED(hr));
    hr = this->videoWindow->put_Owner(ownerHwnd);
    n_assert(SUCCEEDED(hr));
    hr = this->videoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    n_assert(SUCCEEDED(hr));
    hr = this->videoWindow->SetWindowPosition(videoLeft, videoTop, videoWidth, videoHeight);
    n_assert(SUCCEEDED(hr));
    hr = this->videoWindow->put_MessageDrain(ownerHwnd);
    n_assert(SUCCEEDED(hr));
    hr = this->videoWindow->HideCursor(OATRUE);
    n_assert(SUCCEEDED(hr));

    // start playback
    hr = this->mediaControl->Run();
    n_assert(SUCCEEDED(hr));

    this->firstFrame = true;
    return nVideoServer::PlayFile(filename);
}

//------------------------------------------------------------------------------
/**
*/
void
nDShowServer::Stop()
{
    n_assert(this->IsPlaying());

    // end dialog box mode in gfx server
    this->refGfxServer->LeaveDialogBoxMode();

    if (this->basicVideo)
    {
        this->basicVideo->Release();
        this->basicVideo = 0;
    }
    if (this->videoWindow)
    {
        this->videoWindow->Release();
        this->videoWindow = 0;
    }
    if (this->mediaControl)
    {
        this->mediaControl->Release();
        this->mediaControl = 0;
    }
    if (this->mediaEvent)
    {
        this->mediaEvent->Release();
        this->mediaEvent = 0;
    }
    if (this->graphBuilder)
    {
        this->graphBuilder->Release();
        this->graphBuilder = 0;
    }

    HWND hwnd = (HWND) this->refHwnd->GetI();
    ShowWindow(hwnd, SW_RESTORE);

    nVideoServer::Stop();
}

//------------------------------------------------------------------------------
/**
*/
void
nDShowServer::Trigger()
{
    n_assert(this->IsOpen());
    if (this->IsPlaying())
    {
        n_assert(this->mediaEvent);

        // check for media events
        long eventCode;
        LONG_PTR param1;
        LONG_PTR param2;
        while (this->mediaEvent && (S_OK == this->mediaEvent->GetEvent(&eventCode, &param1, &param2, 0)))
        {
            if ((eventCode == EC_COMPLETE) && this->isPlaying)
            {
                this->Stop();
                break;
            }
        }

        // check for Nebula input events (but not in the first playing frame
        if (this->firstFrame)
        {
            this->firstFrame = false;
        }
        else
        {
            nInputEvent* inputEvent;
            for (inputEvent = this->refInputServer->FirstEvent();
                inputEvent != 0;
                inputEvent = this->refInputServer->NextEvent(inputEvent))
            {
                if (inputEvent->GetType() == N_INPUT_KEY_DOWN)
                {
                    this->Stop();
                    break;
                }
            }
        }
    }
}


