#ifndef N_CAPTURESERVER_H
#define N_CAPTURESERVER_H
//------------------------------------------------------------------------------
/**
    @class nCaptureServer
    @ingroup Misc

    @brief Provide continuous frame capture to file at a fixed frame rate.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nCaptureServer : public nRoot
{
public:
    /// constructor
    nCaptureServer();
    /// destructor
    virtual ~nCaptureServer();
    /// get instance pointer
    static nCaptureServer* Instance();
    /// set base directory
    void SetBaseDirectory(const nString& n);
    /// get base directory
    const nString& GetBaseDirectory() const;
    /// set capture frame rate in seconds (def is 1/25)
    void SetFrameTime(nTime t);
    /// get capture frame rate in seconds
    nTime GetFrameTime() const;
    /// start a new capture session
    bool Start();
    /// stop the current capture session
    void Stop();
    /// toggle capturing
    void Toggle();
    /// return true if currently capturing
    bool IsCapturing() const;
    /// trigger the capture server, call after rendering is complete
    bool Trigger();
    /// create a tiled screen shot
    bool SaveTiledScreenShot(int numTilesX, int numTilesY);

private:
    /// find next session index
    void SetToNextSessionIndex();
    /// return full directory name of current session
    nString GetSessionDirectoryPath();
    /// capture the current frame
    bool CaptureFrame();
    /// create session directory for new session
    bool CreateSessionDirectory();
    /// Sets the renderPath offset variable to the right value
    void SetCorrectRenderpathOffset(int numTilesX, int numTilesY, int tileXNum, int tileYNum, bool reset);

    static nCaptureServer* Singleton;

    bool isCapturing;
    nTime frameTime;
    nString baseDirName;
    int sessionIndex;
    int frameIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCaptureServer*
nCaptureServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCaptureServer::SetBaseDirectory(const nString& n)
{
    this->baseDirName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nCaptureServer::GetBaseDirectory() const
{
    return this->baseDirName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCaptureServer::SetFrameTime(nTime t)
{
    this->frameTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nCaptureServer::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCaptureServer::IsCapturing() const
{
    return this->isCapturing;
}


//------------------------------------------------------------------------------
#endif

