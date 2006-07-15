//------------------------------------------------------------------------------
//  nopenalobj.h
//  (C) 2004 Bang, Chang Kyu.
//------------------------------------------------------------------------------
#ifndef N_OPENALOBJ_H
#define N_OPENALOBJ_H

#include "audio3/nsound3.h"
#include "nopenal/nopenalresource.h"

class nOpenALServer;

//-----------------------------------------------------------------------------
/**
    @class nOpenALObj
    @ingroup NOpenALContribModule

    @brief
    @note 
*/
class nOpenALObj: public nSound3
{
public:
    /// constructor
    nOpenALObj();
    /// destructor
    virtual ~nOpenALObj();

    /// start the sound
    virtual void Start();
    /// stop the sound
    virtual void Stop();
    /// update the sound
    virtual void Update();
    /// set the playback volume
    virtual void SetVolume(float v);
    /// get the playback volume
    virtual float GetVolume() const;

protected:
    /// load the resource 
    virtual bool LoadResource();
    /// unload the resource
    virtual void UnloadResource();

    unsigned    m_handle;
    bool bActive;

private:
    nAutoRef<nOpenALServer> refSoundServer;
    nRef<nOpenALResource> refSoundResource;
};

#endif /*N_OPENALOBJ_H*/
