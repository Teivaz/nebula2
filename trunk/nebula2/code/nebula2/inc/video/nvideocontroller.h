#ifndef N_VIDEOCONTROLLER_H
#define N_VIDEOCONTROLLER_H
//------------------------------------------------------------------------------
/**
    *** OBSOLETE *** OBSOLETE *** OBSOLETE ***

    @class nVideoController
    @ingroup Video

    Playback controller to play a single video without commitment to a particular
    implementation.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nautoref.h"
#include "kernel/nenv.h"
#include "util/nstring.h"

class nTexture2;

//------------------------------------------------------------------------------
class nVideoController
{
public:
    /// Construct.
    nVideoController();
    /// Destruct.
    virtual ~nVideoController() = 0;

    /// Set video filename to `v'.
    void SetVideoName(const nString& v);
    /// Filename of associated video.
    const nString& VideoName() const;

    /// Enable/disable playback interruption via user input.
    void SetInterruptible(bool v);
    /// Is video playback interruptible by user input?
    bool IsInterruptible() const;

    /// Enable/disable looping video playback.
    void SetLooping(bool v);
    /// Is looping video playback enabled?
    bool IsLooping() const;

    /// Enable/disable video scaling.
    void SetScaling(bool v);
    /// Is video scaling enabled?
    bool IsScaling() const;

    /// Interrupt video playback.
    void Interrupt();
    /// Interrupted by user input?
    bool Interrupted() const;

    /// Align video in current window.
    void SetPosition(const vector2& uleft, const vector2& lright);
    /// Upper left corner of video in current window.
    const vector2& UpperLeft() const;
    /// Lower right corner of video in current window.
    const vector2& LowerRight() const;

    /// Set render target to `v'.
    void SetRenderTarget(nTexture2* v);
    /// Render target if exist.
    nTexture2* RenderTarget() const;
    /// Will video be rendered into render target instead of current window?
    bool HasRenderTarget() const;

    /// Start playing associated video.
    virtual void Start() = 0;
    /// Stop playing associated video.
    virtual void Stop() = 0;
    /// Refresh video playback.
    virtual void Update() = 0;

    /// Is playback of associated video started?
    bool Running() const;

protected:
    /// Window handle.
    nEnv* WindowHandleEnv();
    /// Full path to file.
    nString FullPathName(const nString& fn) const;

private:
    // Data
    vector2 upperLeft;
    vector2 lowerRight;
    nAutoRef<nEnv> refHwnd;
    nString videoName;
    bool interrupted;
    bool interruptible;
    bool looping;
    bool running;
    bool scaling;
    nTexture2* texture;

};

//------------------------------------------------------------------------------
inline
void
nVideoController::SetVideoName(const nString& v)
{
    n_assert(v != "");
    this->videoName = v;
}
//------------------------------------------------------------------------------
inline
const nString&
nVideoController::VideoName() const
{
    return this->videoName;
}
//------------------------------------------------------------------------------
inline
void
nVideoController::SetInterruptible(bool v)
{
    this->interruptible = v;
}
//------------------------------------------------------------------------------
inline
bool
nVideoController::IsInterruptible() const
{
    return this->interruptible;
}
//------------------------------------------------------------------------------
inline
void
nVideoController::SetLooping(bool v)
{
    this->looping = v;
}
//------------------------------------------------------------------------------
inline
bool
nVideoController::IsLooping() const
{
    return this->looping;
}
//------------------------------------------------------------------------------
inline
void
nVideoController::SetScaling(bool v)
{
    this->scaling = v;
}
//------------------------------------------------------------------------------
inline
bool
nVideoController::IsScaling() const
{
    return this->scaling;
}
//------------------------------------------------------------------------------
inline
void
nVideoController::Interrupt()
{
    n_assert(Running() && IsInterruptible());
    this->interrupted = true;
}
//------------------------------------------------------------------------------
inline
bool
nVideoController::Interrupted() const
{
    return this->interrupted;
}
//------------------------------------------------------------------------------
inline
void
nVideoController::SetPosition(const vector2& uleft, const vector2& lright)
{
    this->upperLeft = uleft;
    this->lowerRight = lright;
}
//------------------------------------------------------------------------------
inline
const vector2&
nVideoController::UpperLeft() const
{
    return this->upperLeft;
}
//------------------------------------------------------------------------------
inline
const vector2&
nVideoController::LowerRight() const
{
    return this->lowerRight;
}
//------------------------------------------------------------------------------
inline
bool
nVideoController::Running() const
{
    return this->running;
}
//------------------------------------------------------------------------------
inline
nEnv*
nVideoController::WindowHandleEnv()
{
    return refHwnd.get();
}
//------------------------------------------------------------------------------
#endif

