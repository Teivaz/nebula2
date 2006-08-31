#ifndef VFX_LIGHTEFFECT_H
#define VFX_LIGHTEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::LightEffect

    An effect which renders a light entity at its position.

    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"
#include "foundation/ptr.h"
#include "graphics/lightentity.h"
#include "util/nanimkey.h"
#include "util/nanimkeyarray.h"

/// Linear interpolation for two nLights, needed for the LightEffect.
/// FIXME: This implementation is not complete; it only interpolates the
///        diffuse, specular, ambient, shadow light mask and translation
//         components of an nLight (enough for point lights).
void nAnimKey<nLight>::Lerp(const nLight& light0, const nLight& light1, float l);

namespace VFX
{

class LightEffect
    : public Effect
{
    DeclareRtti;
    DeclareFactory(LightEffect);
public:
    /// constructor
    LightEffect();
    /// destructor
    virtual ~LightEffect();

    /// set light
    //void SetLight(const nLight& light);
    /// get light
    //const nLight& GetLight() const;

    /// clear all lights
    void ClearLights();
    /// set lights for animation
    void AddLight(const nTime& time, const nLight& light);
    /// set lights for animation
    void SetLights(const nArray<nTime>& times, const nArray<nLight>& lights);
    /// set constant light
    void SetLight(const nLight& light);

    /// set the animation loop type
    void SetLoopType(nAnimLoopType::Type loopType);
    /// return the animation loop type
    nAnimLoopType::Type GetLoopType() const;

    /// start the effect
    virtual void OnStart();
    /// deactivate the effect
    virtual void OnDeactivate();
    /// trigger the effect
    virtual void OnFrame();

private:
    nAnimKeyArray<nAnimKey<nLight> > lights;
    nAnimKey<nLight> currLightAnimKey;
    Ptr<Graphics::LightEntity> lightEntity;
    nAnimLoopType::Type loopType;
}; // class LightEffect

//------------------------------------------------------------------------------
/**
*/
inline
void
LightEffect::SetLoopType(nAnimLoopType::Type loopType)
{
    this->loopType = loopType;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimLoopType::Type
LightEffect::GetLoopType() const
{
    return this->loopType;
}

}; // namespace VFX

//------------------------------------------------------------------------------
#endif