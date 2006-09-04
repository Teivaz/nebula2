#ifndef N_DSHOWCONTROLLER_H
#define N_DSHOWCONTROLLER_H
//------------------------------------------------------------------------------
/**
    *** OBSOLETE *** OBSOLETE *** OBSOLETE ***

    @class nDShowController
    @ingroup Video

    Playback controller of a single video implemented with DirectShow.

    (C) 2005 Radon Labs GmbH
*/
#include "video/nvideocontroller.h"

struct IBasicVideo;
struct IGraphBuilder;
struct IMediaControl;
struct IMediaEvent;
struct IVideoWindow;
struct IBaseFilter;
struct IVMRFilterConfig9;
struct IVMRSurfaceAllocatorNotify9;
class nAllocatorPresenter;

//------------------------------------------------------------------------------
class nDShowController : public nVideoController
{
public:
    /// Construct.
    nDShowController();
    /// Destruct.
    virtual ~nDShowController();

    /// Start playing associated video.
    virtual void Start();
    /// Stop playing associated video.
    virtual void Stop();
    /// Refresh video playback.
    virtual void Update();

private:
    /// Start playing in window.
    void StartWindowed();
    /// Start playing in custom render target (texture).
    void StartRenderless();

    // Data
    IBasicVideo* basicVideo;
    IGraphBuilder* graphBuilder;
    IMediaControl* mediaControl;
    IMediaEvent* mediaEvent;
    IVideoWindow* videoWindow;
    IBaseFilter* baseFilter;
    IVMRFilterConfig9* filterConfig;
    IVMRSurfaceAllocatorNotify9* allocatorNotify;
    nAllocatorPresenter* allocator;
    DWORD userID;
};

//------------------------------------------------------------------------------
#endif

