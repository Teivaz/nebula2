#ifndef N_SOUNDRESOURCE_H
#define N_SOUNDRESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nSoundResource
    @ingroup Audio3

    A sound resource is a container for sound data which can be played back
    by the nAudioServer3 subsystem. The sound may be static or streaming,
    oneshot or looping. A sound resource should be able play itself
    several times simultaneously, the intended number of parallel
    "tracks" can be set by the user before opening the resource.

    Sound resources are generally shared and are referenced by
    nSound3 objects (there should be one nSound3 object per "sound instance",
    but several nSound3 objects should reference the same nSoundResource object,
    if the nSound3 objects sound the same).

    (C) 2003 RadonLabs GmbH
*/
#include "resource/nresource.h"

//------------------------------------------------------------------------------
class nSoundResource : public nResource
{
public:
    /// constructor
    nSoundResource();
    /// destructor
    virtual ~nSoundResource();
    /// set the number of simultaneous "playback tracks" this sound should support
    void SetNumTracks(int t);
    /// get number of tracks
    int GetNumTracks() const;
    /// set the ambient flag (ambient sounds are non-positional)
    void SetAmbient(bool b);
    /// get the ambient flag
    bool GetAmbient() const;
    /// set static/streaming type
    void SetStreaming(bool b);
    /// get static/streaming type
    bool GetStreaming() const;
    /// set the looping behaviour
    void SetLooping(bool b);
    /// get the looping behaviour
    bool GetLooping() const;

protected:
    int numTracks;
    bool ambient;
    bool streaming;
    bool looping;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSoundResource::SetNumTracks(int t)
{
    this->numTracks = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSoundResource::GetNumTracks() const
{
    return this->numTracks;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSoundResource::SetAmbient(bool b)
{
    this->ambient = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSoundResource::GetAmbient() const
{
    return this->ambient;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSoundResource::SetStreaming(bool b)
{
    this->streaming = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSoundResource::GetStreaming() const
{
    return this->streaming;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSoundResource::SetLooping(bool b)
{
    this->looping = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSoundResource::GetLooping() const
{
    return this->looping;
}

//------------------------------------------------------------------------------
#endif

