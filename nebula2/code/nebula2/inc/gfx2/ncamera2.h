#ifndef N_CAMERA2_H
#define N_CAMERA2_H
//------------------------------------------------------------------------------
/**
    @class nCamera2
    @ingroup NebulaGraphicsSystem

    Hold camera attributes for nGfxServer2.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nCamera2
{
public:
    /// constructor
    nCamera2();
    /// set angle of view
    void SetAngleOfView(float a);
    /// get angle of view
    float GetAngleOfView() const;
    /// set horizontal/vertical aspect ratio (i.e. (4.0 / 3.0))
    void SetAspectRatio(float r);
    /// get aspect ratio
    float GetAspectRatio() const;
    /// set near clip plane
    void SetNearPlane(float v);
    /// get near clip plane
    float GetNearPlane() const;
    /// set far clip plane
    void SetFarPlane(float v);
    /// set far clip plane
    float GetFarPlane() const;

private:
    float angleOfView;
    float aspectRatio;
    float nearPlane;
    float farPlane;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCamera2::nCamera2() :
    angleOfView(60.0f),
    aspectRatio(4.0f / 3.0f),
    nearPlane(0.1f),
    farPlane(500.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCamera2::SetAngleOfView(float a)
{
    this->angleOfView = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCamera2::GetAngleOfView() const
{
    return this->angleOfView;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCamera2::SetAspectRatio(float r)
{
    this->aspectRatio = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCamera2::GetAspectRatio() const
{
    return this->aspectRatio;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCamera2::SetNearPlane(float v)
{
    this->nearPlane = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCamera2::GetNearPlane() const
{
    return this->nearPlane;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCamera2::SetFarPlane(float v)
{
    this->farPlane = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCamera2::GetFarPlane() const
{
    return this->farPlane;
}

//------------------------------------------------------------------------------
#endif
