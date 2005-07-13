//------------------------------------------------------------------------------
//  nsoundobject.h
//
//  (C) 2004 Bang, Chang Kyu
//------------------------------------------------------------------------------
#ifndef N_OPENALHELPER_H
#define N_OPENALHELPER_H

#include "nopenal/nopenalserver.h"
#include "nopenal/nopenalobj.h"
#include "nopenal/nopenalresource.h"

//-----------------------------------------------------------------------------
/**
    @class nOpenALHelper
    @ingroup NOpenALDemoContribModule

    Helper class to demonstrate the usage of nOpenAL package.

    -13-July-05    kims    changed the class name nOpenALCore to nOpenALHelper.
                           also removed some code to compatible it with nApplication
                           based demo application.
*/

//------------------------------------------------------------------------------
class nOpenALHelper
{
public:
    /// constructor
    nOpenALHelper();
    /// destructor
    ~nOpenALHelper();
    /// update listener's position and direction
    void ListenerPosition (float *pos, float *angle);
    /// specify the position of the source.
    void SetSourcePosition (nOpenALObj *oalobj, float *);
    /// specify the velocity of the source.
    void SetSourceVelocity (nOpenALObj *oalobj, float *velocity);
    /// steps through the global environment presets if available
    int  IncrementEnv();
    /// load sound file.
    nOpenALObj* LoadFile (char*, char*);
    /// load and play sound file.
    nOpenALObj* PlayFile (char*, char*);

private:
    /// maximum number of the sound objects.
    enum { MAX_SOUNDOBJECT = 256 };
    /// sound objects.
    nOpenALObj*  soundObjs[MAX_SOUNDOBJECT];
    /// number of the sound object which aleady loaded.
    unsigned int soundObjCounts;

};

//------------------------------------------------------------------------------
#endif // N_OPENAL_CORE_H
