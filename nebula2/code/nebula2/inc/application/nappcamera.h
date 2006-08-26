#ifndef N_APPCAMERA_H
#define N_APPCAMERA_H
//------------------------------------------------------------------------------
/**
    @class nAppCamera
    @ingroup Application

    @brief A simple application camera object. Derive subclasses if you
    need more advanced camera behaviour.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/transform44.h"

//------------------------------------------------------------------------------
class nAppCamera
{
public:
    /// constructor
    nAppCamera();
    /// destructor
    virtual ~nAppCamera();
    /// access to the application camera's transform object
    transform44& Transform();
    /// return current view matrix
    const matrix44& GetViewMatrix();
    /// set the current frame time
    void SetFrameTime(nTime t);
    /// get the current frame time
    nTime GetFrameTime() const;
    /// reset the camera to the default position and orientation
    virtual void Reset();
    /// do per-frame-work
    virtual void Trigger();

private:
    transform44 transform;
    nTime frameTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
transform44&
nAppCamera::Transform()
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nAppCamera::GetViewMatrix()
{
    return this->transform.getmatrix();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppCamera::SetFrameTime(nTime t)
{
    this->frameTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nAppCamera::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
#endif
