#ifndef N_DSHOWSERVER_H
#define N_DSHOWSERVER_H
//------------------------------------------------------------------------------
/**
    A Nebula2 video server running on top of DirectShow.

    (C) 2004 RadonLabs GmbH
*/
#include "video/nvideoserver.h"
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "kernel/nautoref.h"
#include "kernel/nenv.h"
#include <dshow.h>

//------------------------------------------------------------------------------
class nDShowServer : public nVideoServer
{
public:
    /// constructor
    nDShowServer();
    /// destructor
    virtual ~nDShowServer();
    /// open the video server
    virtual bool Open();
    /// close the video server
    virtual void Close();
    /// start playback of a video file
    virtual bool PlayFile(const char* filename);
    /// stop playback
    virtual void Stop();
    /// per-frame trigger
    virtual void Trigger();

private:
    nAutoRef<nEnv> refHwnd;
    nAutoRef<nInputServer> refInputServer;
    nAutoRef<nGfxServer2> refGfxServer;
    IGraphBuilder* graphBuilder;
    IMediaControl* mediaControl;
    IMediaEvent* mediaEvent;
    IVideoWindow* videoWindow;
    IBasicVideo* basicVideo;
    bool firstFrame;
};

//------------------------------------------------------------------------------
#endif

