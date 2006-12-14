#ifndef N_DSHOWSERVER_H
#define N_DSHOWSERVER_H
//------------------------------------------------------------------------------
/**
    @class nDShowServer
    @ingroup Video

    @brief A Nebula2 video server running on top of DirectShow.

    (C) 2004 RadonLabs GmbH
*/
#include "video/nvideoserver.h"
#include "gfx2/ngfxserver2.h"
#include "input/ninputserver.h"
#include "kernel/nautoref.h"
#include "kernel/nenv.h"
#include <dshow.h>
#include "resource/nresourceserver.h"

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
    /// create new video player
    virtual nVideoPlayer* NewVideoPlayer(nString name);
    /// delete video player
    virtual void DeleteVideoPlayer(nVideoPlayer* player);

private:
    nAutoRef<nEnv> refHwnd;
    nAutoRef<nInputServer> refInputServer;
    IGraphBuilder* graphBuilder;
    IMediaControl* mediaControl;
    IMediaEvent* mediaEvent;
    IVideoWindow* videoWindow;
    IBasicVideo* basicVideo;
    bool firstFrame;
    nTime oldTime;
    bool timeSet;
};

//------------------------------------------------------------------------------
#endif

