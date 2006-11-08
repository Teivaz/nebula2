//------------------------------------------------------------------------------
//  vfx/shakeeffecthelper.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/shakeeffecthelper.h"
#include "vfx/server.h"

namespace VFX
{

//------------------------------------------------------------------------------
/**
*/
ShakeEffectHelper::ShakeEffectHelper() :
    maxDisplacement(1.0f, 1.0f, 1.0f),
    maxTumble(10.0f, 10.0f, 10.0f),
    lastTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ShakeEffectHelper::Update()
{
    // only update displace and tumble at some relatively low frame rate
    // this prevents the shake effect from "flickering" if the
    // frame rate is very high
    nTime curTime = VFX::Server::Instance()->GetTime();
    if ((0.0 == this->lastTime) || ((curTime - this->lastTime) > 0.01))
    {
        this->lastTime = curTime;

        // compute a random displacement vector
        this->curDisplace.set(((n_rand() * 2.0f) - 1.0f) * this->maxDisplacement.x,
                              ((n_rand() * 2.0f) - 1.0f) * this->maxDisplacement.y,
                              ((n_rand() * 2.0f) - 1.0f) * this->maxDisplacement.z);


        // compute random tumble angles
        this->curTumble.set(((n_rand() * 2.0f) - 1.0f) * this->maxTumble.x,
                            ((n_rand() * 2.0f) - 1.0f) * this->maxTumble.y,
                            ((n_rand() * 2.0f) - 1.0f) * this->maxTumble.z);
    }

    // get current accumulated shake intensity at my position
    float shakeIntensity = VFX::Server::Instance()->ComputeShakeIntensityAtPosition(this->cameraMatrix44.pos_component());

    // update the current incoming camera matrix by the displace and tumble vectors
    if (shakeIntensity > 0.0f)
    {
        matrix44 shakeMatrix;
        shakeMatrix.rotate_x(this->curTumble.x * shakeIntensity);
        shakeMatrix.rotate_y(this->curTumble.y * shakeIntensity);
        shakeMatrix.rotate_z(this->curTumble.z * shakeIntensity);
        shakeMatrix.translate(this->cameraMatrix33 * (this->curDisplace * shakeIntensity));
        this->shakeCameraMatrix = shakeMatrix * this->cameraMatrix44;
    }
    else
    {
        this->shakeCameraMatrix = this->cameraMatrix44;
    }
}

} // namespace VFX
