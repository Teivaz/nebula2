//------------------------------------------------------------------------------
//  nopenalserverr.h
//  (C) 2004 Bang, Chang Kyu.
//------------------------------------------------------------------------------
#ifndef N_OPENALRESOURCE_H
#define N_OPENALRESOURCE_H

#include "audio3/nsoundresource.h"
#include <vorbis/vorbisfile.h>

class nOpenALServer;
class nOpenALObj;

//-----------------------------------------------------------------------------
/**
    @class nOpenALResource
    @ingroup NOpenALContribModule
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

    void load_ogg_file(const nString&, const unsigned int&);
    void read_ogg_block(const unsigned int&, OggVorbis_File&);
};

#endif /*N_OPENALRESOURCE*/
