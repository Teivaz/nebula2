//------------------------------------------------------------------------------
//  bbsound.h
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#ifndef N_CSOUND_H
#define N_CSOUND_H

#include "kernel/nroot.h"
class nSound3;

//-----------------------------------------------------------------------------
/**
    @class BBSound
    @ingroup BombsquadBruceContribModule

    @brief
    Encapsulates a sound scriptably.
*/

//------------------------------------------------------------------------------
class BBSound : public nRoot
{
public:
    /// constructor
    BBSound();
    /// destructor
    ~BBSound();

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
