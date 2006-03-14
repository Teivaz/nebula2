#ifndef VFX_SHAKEEFFECTHELPER_H
#define VFX_SHAKEEFFECTHELPER_H
//------------------------------------------------------------------------------
/**
    @class VFX::ShakeEffectHelper

    This helper class computes camera displacement vector to produce
    a camera shake. Use this class in your camera controller to let the
    camera respond to shake effects.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/matrix.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
namespace VFX
{
class ShakeEffectHelper
{
public:
    /// constructor
    ShakeEffectHelper();
    /// set current camera transformation
    void SetCameraTransform(const matrix44& m);
    /// set desired max displacement along camera axes
    void SetMaxDisplacement(const vector3& d);
    /// set desired maximum tumble angles in degrees
    void SetMaxTumble(const vector3& a);
    /// apply displace and tumble to camera matrix
    void Update();
    /// get new camera transform (valid after Update()).
    const matrix44& GetShakeCameraTransform() const;

private:
    matrix44 cameraMatrix44;
    matrix33 cameraMatrix33;
    vector3 maxDisplacement;
    vector3 maxTumble;
    nTime lastTime;
    vector3 curDisplace;
    vector3 curTumble;
    matrix44 shakeCameraMatrix;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffectHelper::SetCameraTransform(const matrix44& m)
{
    this->cameraMatrix44 = m;
    this->cameraMatrix33.set(m.x_component(), m.y_component(), m.z_component());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffectHelper::SetMaxDisplacement(const vector3& d)
{
    this->maxDisplacement = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ShakeEffectHelper::SetMaxTumble(const vector3& a)
{
    this->maxTumble.x = n_deg2rad(a.x);
    this->maxTumble.y = n_deg2rad(a.y);
    this->maxTumble.z = n_deg2rad(a.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
ShakeEffectHelper::GetShakeCameraTransform() const
{
    return this->shakeCameraMatrix;
}

}; // namespace VFX
//------------------------------------------------------------------------------
#endif
