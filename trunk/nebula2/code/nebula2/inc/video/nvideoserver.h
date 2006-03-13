#ifndef N_VIDEOSERVER_H
#define N_VIDEOSERVER_H
//------------------------------------------------------------------------------
/**
    @class nVideoServer
    @ingroup Video

    @brief Server object to playback video streams.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "video/nvideoplayer.h"

//------------------------------------------------------------------------------
class nVideoServer : public nRoot
{
public:
    /// constructor
    nVideoServer();
    /// destructor
    virtual ~nVideoServer();
    /// get instance pointer
    static nVideoServer* Instance();
    /// open the video server
    virtual bool Open();
    /// close the video server
    virtual void Close();
    /// currently open?
    bool IsOpen() const;
    /// enable/disable scaling to screen size
    void SetEnableScaling(bool b);
    /// get scaling enabled flag
    bool GetEnableScaling() const;
    /// start playback of a video file
    virtual bool PlayFile(const char* filename);
    /// stop playback
    virtual void Stop();
    /// currently playing?
    bool IsPlaying() const;
    /// per-frame trigger
    virtual void Trigger();
    /// create new video player
    virtual nVideoPlayer*   NewVideoPlayer(nString name);
    /// delete video player
    virtual void   DeleteVideoPlayer(nVideoPlayer* player);

protected:
    static nVideoServer* Singleton;

    nArray<nVideoPlayer*>   videoPlayers;

    bool isOpen;
    bool isPlaying;
    bool scalingEnabled;
};

//------------------------------------------------------------------------------
/**
*/
inline
nVideoServer*
nVideoServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVideoServer::SetEnableScaling(bool b)
{
    this->scalingEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVideoServer::GetEnableScaling() const
{
    return this->scalingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVideoServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVideoServer::IsPlaying() const
{
    return this->isPlaying;
}

//------------------------------------------------------------------------------
#endif

