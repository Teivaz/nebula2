//------------------------------------------------------------------------------
//  nopenalserverr.h
//  (C) 2004 Bang, Chang Kyu.
//------------------------------------------------------------------------------
#ifndef N_OPENALRESOURCE_H
#define N_OPENALRESOURCE_H

#include "audio3/nsoundresource.h"

class nOpenALServer;
class nOpenALObj;

//-----------------------------------------------------------------------------
/**
    @class nOpenALServer
    @ingroup nOpenALGroup

    @brief
    @note 
*/
//------------------------------------------------------------------------------
class nOpenALResource : public nSoundResource
{
public:
    /// constructor
    nOpenALResource();
    /// destructor
    virtual ~nOpenALResource();

    unsigned getHANDLE() { return m_handle; }

protected:
    /// load the resource (sets the valid flag)
    virtual bool LoadResource();
    /// unload the resource (clears the valid flag)
    virtual void UnloadResource();

private:
    nAutoRef<nOpenALServer> refSoundServer;
    unsigned    m_handle;
};

#endif /*N_OPENALRESOURCE*/
