#ifndef N_LISTENER3_H
#define N_LISTENER3_H
//------------------------------------------------------------------------------
/**
    @class nListener3
    @ingroup Audio3

    Define listener properties for audio subsystem 3.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/matrix.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class nListener3
{
public:
    /// constructor
    nListener3();
    /// destructor
    ~nListener3();
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
/**
*/
inline
nListener3::nListener3() :
    rollOffFactor(1.0f),
    dopplerFactor(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nListener3::~nListener3()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener3::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nListener3::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener3::SetVelocity(const vector3& v)
{
    this->velocity = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nListener3::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener3::SetRollOffFactor(float f)
{
    this->rollOffFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListener3::GetRollOffFactor() const
{
    return this->rollOffFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nListener3::SetDopplerFactor(float f)
{
    this->dopplerFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nListener3::GetDopplerFactor() const
{
    return this->dopplerFactor;
}

//------------------------------------------------------------------------------
#endif    
