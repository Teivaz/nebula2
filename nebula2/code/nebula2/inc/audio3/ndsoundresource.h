#ifndef N_DSOUNDRESOURCE_H
#define N_DSOUNDRESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nDSoundResource

    An implementation of the nSoundResource class on top of DirectSound.
    
    (C) 2003 RadonLabs GmbH
*/
#include "audio3/nsoundresource.h"

#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

class nDSoundServer3;
class nDSound3;

//------------------------------------------------------------------------------
class nDSoundResource : public nSoundResource
{
public:
    /// constructor
    nDSoundResource();
    /// destructor
    virtual ~nDSoundResource();

    // kernelServer static
    static nKernelServer* kernelServer;

protected:
    /// load the resource (sets the valid flag)
    virtual bool LoadResource();
    /// unload the resource (clears the valid flag)
    virtual void UnloadResource();

private:
    nAutoRef<nDSoundServer3> refSoundServer;

    // direct sound object
public:
    // accessor
    void        setSound3( nDSound3 *_dsSound ) { dsSound = _dsSound; }
    nDSound3 *  getSound3( void ) { return dsSound; }

private:
    // member
    nDSound3 *dsSound;
};

//------------------------------------------------------------------------------
#endif
