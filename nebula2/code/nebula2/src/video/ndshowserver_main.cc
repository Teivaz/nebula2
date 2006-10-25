//------------------------------------------------------------------------------
//  ndshowserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "video/ndshowserver.h"
#include "video/noggtheoraplayer.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"

#include "gfx2/nd3d9server.h"

nNebulaClass(nDShowServer, "nvideoserver");

//------------------------------------------------------------------------------
/**
*/
nDShowServer::nDShowServer() :
    refInputServer("/sys/servers/input"),
    refHwnd("/sys/env/hwnd"),
    graphBuilder(0),
    mediaControl(0),
    mediaEvent(0),
    videoWindow(0),
    basicVideo(0),
    firstFrame(false),
    timeSet(false)
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
    n_assert(0 == this->basicVideo);
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

    while (videoPlayers.Size()>0)
    {
        this->DeleteVideoPlayer(videoPlayers.At(0));
    }
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
    n_assert(0 == this->basicVideo);

    HRESULT hr;

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginFrame();
    gfxServer->BeginScene();
    gfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0);
    gfxServer->EndScene();
    gfxServer->PresentScene();
    gfxServer->EndFrame();

    gfxServer->EnterDialogBoxMode();

    gfxServer->BeginFrame();
    gfxServer->BeginScene();
    gfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0);
    gfxServer->EndScene();
    gfxServer->PresentScene();
    gfxServer->EndFrame();

    // mangle Nebula path into absolute path wide character string
    wchar_t widePath[N_MAXPATH];
    nString mangledPath = nFileServer2::Instance()->ManglePath(filename);
    mbstowcs(widePath, mangledPath.Get(), mangledPath.Length() + 1);

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

    // compute video window size
    OAHWND ownerHwnd = (OAHWND) this->refHwnd->GetI();
    RECT rect;
    GetClientRect((HWND)ownerHwnd, &rect);
    LONG videoLeft, videoTop, videoWidth, videoHeight;
    if (this->GetEnableScaling())
    {
        // scale to full-screen....
        videoLeft   = 0;
        videoTop    = 0;
        videoWidth  = rect.right;
        videoHeight = rect.bottom;
    }
    else
    {
        // render video in original size, centered
        hr = this->basicVideo->GetVideoSize(&videoWidth, &videoHeight);
        n_assert(SUCCEEDED(hr));

        // get public window handle, which has been initialized by the gfx subsystem
        videoLeft = ((rect.right + rect.left) - videoWidth) / 2;
        videoTop = ((rect.bottom + rect.top) - videoHeight) / 2;
    }

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
    nGfxServer2::Instance()->LeaveDialogBoxMode();

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

    nTime time = nTimeServer::Instance()->GetTime();
    if (!timeSet)
    {
        oldTime=time;
        timeSet = true;
    };

    // check if time-reset occured
    if (time < oldTime)
    {
        oldTime = time;
        // rewind players
        int i;
        for (i = 0; i<videoPlayers.Size() ; i++)
        {
            nVideoPlayer*   currentPlayer = videoPlayers.At(i);
            if (currentPlayer->IsOpen())
                currentPlayer->Rewind();
        };

    };

    nTime deltaTime = time - oldTime;
    oldTime = time;

    int i;
    for (i = 0; i<videoPlayers.Size() ; i++)
    {
        nVideoPlayer*   currentPlayer = videoPlayers.At(i);
        if (currentPlayer->IsOpen())
            currentPlayer->Decode(deltaTime);
    };

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
                if (this->IsPlaying())
                {
                    this->Stop();
                }
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
                if ((inputEvent->GetType() == N_INPUT_KEY_DOWN) &&
                    ((inputEvent->GetKey() == N_KEY_ESCAPE) ||
                     (inputEvent->GetKey() == N_KEY_SPACE)))
                {
                    if (this->IsPlaying())
                    {
                        this->Stop();
                    }
                    break;
                }
            }
        }
    }
}


//------------------------------------------------------------------------------
/**
*/
nVideoPlayer*
nDShowServer::NewVideoPlayer(nString name)
{
    nVideoPlayer*   player = (nVideoPlayer*) nResourceServer::Instance()->NewResource("noggtheoraplayer", name.Get(), nResource::Other);
    player->SetFilename(name);
    videoPlayers.PushBack(player);
    return player;
}

//------------------------------------------------------------------------------
/**
   delete video player
*/
void
nDShowServer::DeleteVideoPlayer(nVideoPlayer* player)
{
    n_assert(player);
    int i;
    for (i = 0; i<videoPlayers.Size() ; i++)
        if (videoPlayers.At(i) == player)
        {
            videoPlayers.Erase(i);
            break;
        };
    if (player->IsOpen())
        player->Close();
    player->Release();
};
