#ifndef N_DSOUNDRESOURCE_H
#define N_DSOUNDRESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nDSoundResource
    @ingroup Audio3
    @brief An implementation of the nSoundResource class on top of DirectSound.

    (C) 2003 RadonLabs GmbH
*/
#include "audio3/nsoundresource.h"
#include "audio3/dsutil.h"

class nDSoundServer3;

//------------------------------------------------------------------------------
class nDSoundResource : public nSoundResource
{
public:
    /// constructor
    nDSoundResource();
    /// destructor
    virtual ~nDSoundResource();
    /// get pointer to the embedded CSound object
    CSound* GetCSoundPtr();
    /// get the notification event handle
    HANDLE GetNotifyEvent();

protected:
    /// load the resource (sets the valid flag)
    virtual bool LoadResource();
    /// unload the resource (clears the valid flag)
    virtual void UnloadResource();

private:
    nAutoRef<nDSoundServer3> refSoundServer;
    CSound* dsSound;
    CStreamingSound* dsStreamingSound;
};

//------------------------------------------------------------------------------
/**
*/
inline
CSound*
nDSoundResource::GetCSoundPtr()
{
    if (this->streaming)
    {
        return this->dsStreamingSound;
    }
    return this->dsSound;
}

//------------------------------------------------------------------------------
#endif
