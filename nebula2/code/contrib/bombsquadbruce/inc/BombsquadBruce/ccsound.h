//------------------------------------------------------------------------------
//  ccsound.h
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#ifndef N_CSOUND_H
#define N_CSOUND_H

#include "kernel/nroot.h"
class nSound3;

//-----------------------------------------------------------------------------
/**
    @class CCSound
    @ingroup BombsquadBruceContribModule

    @brief
    Encapsulates a sound scriptably.
*/

//------------------------------------------------------------------------------
class CCSound : public nRoot
{
public:
    /// constructor
    CCSound();
    /// destructor
    ~CCSound();

    bool Init( const char* filename, bool loop, bool ambient );
    void Play();
    void Stop();

    void SetPosition( const vector3& );
    void SetVolume( float );
private:
    nSound3 * m_pResource;
};

//------------------------------------------------------------------------------
#endif // N_CSOUND_H
