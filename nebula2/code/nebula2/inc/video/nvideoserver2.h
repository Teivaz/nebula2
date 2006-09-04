#ifndef N_VIDEOSERVER2_H
#define N_VIDEOSERVER2_H
//------------------------------------------------------------------------------
/**
    *** OBSOLETE *** OBSOLETE *** OBSOLETE ***

    @class nVideoServer2
    @ingroup Video

    Server objects to playback video streams via video controllers without
    commitment to a particular implementation.

    (C) 2005 Radon Labs GmbH
*/
#include "input/ninputserver.h"
#include "kernel/nroot.h"

class nVideoController;
class nTexture2;

//------------------------------------------------------------------------------
class nVideoServer2 : public nRoot
{
public:
    /// Construct.
    nVideoServer2();
    /// Destruct.
    virtual ~nVideoServer2();

    /// The one and only instance.
    static nVideoServer2* Instance();

    /// Initialize.
    virtual void Open();
    /// Finish.
    virtual void Close();
    /// Is initialized?
    bool IsOpen() const;

    /// Add video associated with filename `vn'.
    void AddVideo(const nString& vn);
    /// Remove video associated with filename `vn'.
    void RemoveVideo(const nString& vn);
    /// Remove all videos.
    void RemoveVideos();
    /// Does this contain video associated with filename `vn'?
    bool HasVideo(const nString& vn) const;

    /// Enable/disable user interruption of video `vn'.
    void SetInterruptible(const nString& vn, bool v);
    /// Is video `vn' interruptible by user?
    bool IsInterruptible(const nString& vn) const;

    /// Enable/disable playback looping of video `vn'.
    void SetLooping(const nString& vn, bool v);
    /// Is video `vn' looping?
    bool IsLooping(const nString& vn) const;

    /// Enable/disable scaling of video `vn'.
    void SetScaling(const nString& vn, bool v);
    /// Is scaling enabled for video `vn'?
    bool IsScaling(const nString& vn) const;

    /// Align video `vn' in current window.
    void SetVideoPosition(const nString& vn, const vector2& ul, const vector2& lr);

    /// Render video `vn' into target `v'.
    void SetRenderTarget(const nString& vn, nTexture2* v);

    /// Start playback of current video.
    void StartVideo(const nString& vn);
    /// Stop playback of current video.
    void StopVideo(const nString& vn);
    /// Is video `vn' playing?
    bool VideoRunning(const nString& vn) const;

    /// Number of videos available.
    int CountVideos() const;

    /// Update internal status.
    virtual void Trigger();

protected:
    /// New video controller for video associated with filename `vn'.
    virtual nVideoController* MakeController(const nString& vn) const;

private:
    /// Index of controller controlling video associated with filename `vn'.
    int IndexOfController(const nString& vn) const;
    /// Check if gfx server dialog box mode is needed. Enable it if needed otherwise
    /// disable it.
    void CheckDialogBoxMode();

    // Data
    static nVideoServer2* Singleton;
    nAutoRef<nInputServer> refInputServer;
    nArray<nVideoController*> controller;
    bool isOpen;
    bool needCheckDialogBoxMode;
};

//------------------------------------------------------------------------------
inline
nVideoServer2*
nVideoServer2::Instance()
{
    n_assert(Singleton);
    return Singleton;
}
//------------------------------------------------------------------------------
inline
bool
nVideoServer2::IsOpen() const
{
    return this->isOpen;
}
//------------------------------------------------------------------------------
inline
int
nVideoServer2::CountVideos() const
{
    return this->controller.Size();
}
//------------------------------------------------------------------------------
#endif

