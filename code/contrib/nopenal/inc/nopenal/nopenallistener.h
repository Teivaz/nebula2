#ifndef N_OPENALLISTENER_H
#define N_OPENALLISTENER_H
//------------------------------------------------------------------------------
/**
    @class nOpenALListener
    @ingroup NOpenALContribModule

    Define listener properties for audio subsystem 3.
    
    (C) 2003 RadonLabs GmbH
*/
#include "audio3/nlistener3.h"

//------------------------------------------------------------------------------
class nOpenALListener : nListener3
{
public:
    /// constructor
    nOpenALListener();
    /// destructor
    ~nOpenALListener();
    /// set world space transform
    void SetTransform(const matrix44& m);
    /// get world space transform
    const matrix44& GetTransform() const;
    /// set world space velocity
    void SetVelocity(const vector3& v);
    /// get world space velocity
    const vector3& GetVelocity() const;
    /// set rolloff factor
    void SetRollOffFactor(float f);
    /// get rolloff factor
    float GetRollOffFactor() const;
    /// set doppler factor
    void SetDopplerFactor(float f);
    /// get doppler factor
    float GetDopplerFactor() const;

private:
    matrix44 transform;
    vector3 velocity;
    float rollOffFactor;
    float dopplerFactor;
};

//------------------------------------------------------------------------------
#endif // N_OPENALLISTENER_H
