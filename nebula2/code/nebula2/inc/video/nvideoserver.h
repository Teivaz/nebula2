#ifndef N_VIDEOSERVER_H
#define N_VIDEOSERVER_H
//------------------------------------------------------------------------------
/**
    Server object to playback video streams.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nVideoServer : public nRoot
{
public:
    /// constructor
    nVideoServer();
    /// destructor
    virtual ~nVideoServer();
    /// open the video server
    virtual bool Open();
    /// close the video server
    virtual void Close();
    /// currently open?
    bool IsOpen() const;
    /// start playback of a video file
    virtual bool PlayFile(const char* filename);
    /// stop playback
    virtual void Stop();
    /// currently playing?
    bool IsPlaying() const;
    /// per-frame trigger
    virtual void Trigger();

protected:
    bool isOpen;
    bool isPlaying;
};

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

