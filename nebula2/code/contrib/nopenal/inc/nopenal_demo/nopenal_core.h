//------------------------------------------------------------------------------
//  nopenal_core.h
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#ifndef N_OPENAL_CORE_H
#define N_OPENAL_CORE_H

#include "nopenal/nopenalserver.h"
#include "nopenal/nopenalobj.h"
#include "nopenal/nopenalresource.h"

//-----------------------------------------------------------------------------
/**
    @class nOpenALCore
    @ingroup NOpenALDemoContribModule

    @brief
    @note 
*/

//------------------------------------------------------------------------------
class nOpenALCore
{
public:
    /// constructor
    nOpenALCore();
    /// destructor
    ~nOpenALCore();

    void    Init ();
    void    ListenerPosition (float *, float *);
    nOpenALObj*     LoadFile (char *, char *);
    nOpenALObj*     PlayFile (char *, char *);
    void    SetSourcePosition (nOpenALObj *oalobj, float *);
    void    SetSourceVelocity (nOpenALObj *oalobj, float *);
    int     IncrementEnv();

private:
    nOpenALObj*  soundObjs[256];
    unsigned int soundObjCounts;
    nAutoRef<nAudioServer3> refAudioServer;
    nKernelServer* kernelServer;
};

//------------------------------------------------------------------------------
#endif // N_OPENAL_CORE_H
