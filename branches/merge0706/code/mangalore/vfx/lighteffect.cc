//------------------------------------------------------------------------------
//  lighteffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/lighteffect.h"
#include "vfx/server.h"
#include "graphics/server.h"
#include "graphics/level.h"
#include "game/time/gametimesource.h"

//------------------------------------------------------------------------------
/**
    Interpolation function for nLights.

    FIXME: This just interpolates diffuse, specular, ambient and shadowLightMask
           and range of two nLights.
           
           The interpolation of the transform is quite tricky. A useful 
           resource is http://skal.planet-d.net/demo/matrixfaq.htm#Q44.
           This implementation only interpolates the translation, with
           is enough for point lights.
           
           Light type and castShadows is always set to light0's values.
*/
void
nAnimKey<nLight>::Lerp(const nLight& light0, const nLight& light1, float l)
{
    // diffuse
    const vector4& v0 = light0.GetDiffuse();
    const vector4& v1 = light1.GetDiffuse();
    this->value.SetDiffuse(v0 + (v1 - v0) * l);

    // specular
    const vector4& s0 = light0.GetSpecular();
    const vector4& s1 = light1.GetSpecular();
    this->value.SetSpecular(s0 + (s1 - s0) * l);

    // ambient
    const vector4& a0 = light0.GetAmbient();
    const vector4& a1 = light1.GetAmbient();
    this->value.SetAmbient(a0 + (a1 - a0) * l);

    // shadowLightMask
    const vector4& slm0 = light0.GetShadowLightMask();
    const vector4& slm1 = light1.GetShadowLightMask();
    this->value.SetShadowLightMask(slm0 + (slm1 - slm0) * l);

    // range
    float r0 = light0.GetRange();
    float r1 = light1.GetRange();
    this->value.SetRange(r0 + (r1 - r0) * l);

    // type and castShadows
    this->value.SetType(light0.GetType());
    this->value.SetCastShadows(light0.GetCastShadows());

    // transform
    matrix44 m0 = light0.GetTransform();
    matrix44 m1 = light1.GetTransform();

    // translation
    vector3 t0 = m0.pos_component();
    vector3 t1 = m1.pos_component();
    vector3 t  = (t0 + (t1 - t0) * l);
/*
    // rotation
    // FIXME: does this work?
    quaternion q0 = m0.get_quaternion(); // m0 must not be scaled
    quaternion q1 = m1.get_quaternion(); // m0 must not be scaled
    quaternion q  = (q0 + l * (q1 - q0));
*/
    matrix44 m(m0);
    m.set_translation(t);
    this->value.SetTransform(m);
}

namespace VFX
{
ImplementRtti(VFX::LightEffect, VFX::Effect);
ImplementFactory(VFX::LightEffect);

//------------------------------------------------------------------------------
/**
*/
LightEffect::LightEffect()
    : loopType(nAnimLoopType::Clamp)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
LightEffect::~LightEffect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
LightEffect::OnStart()
{
    n_assert(!this->lights.Empty());

    // call parent class
    Effect::OnStart();

    // create and initialize light entity
    this->lightEntity = Graphics::LightEntity::Create();

    float startTime = this->lights.Begin()->GetTime();
    
    nAnimKey<nLight> currLightAnimKey;
    this->lights.Sample(0.0f, this->loopType, currLightAnimKey);
    
    this->lightEntity->SetLight(currLightAnimKey.GetValue());
    this->lightEntity->SetTransform(this->ComputeWorldSpaceTransform());

    Graphics::Server::Instance()->GetLevel()->AttachEntity(this->lightEntity);
}

//------------------------------------------------------------------------------
/**
*/
void
LightEffect::OnDeactivate()
{
    if (this->lightEntity.isvalid())
    {
        // cleanup graphics entity
        Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
        if (graphicsLevel)
        {
            graphicsLevel->RemoveEntity(this->lightEntity);
        }
        this->lightEntity = 0;
    }

    // call parent class
    Effect::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
LightEffect::OnFrame()
{
    Effect::OnFrame();

    if (this->IsPlaying())
    {
        // interpolate between lights
        nTime curTime = this->GetTime();
        nTime timeDiff = (curTime - this->startTime);

        this->lights.Sample((float)timeDiff, this->loopType, this->currLightAnimKey);
        
        this->lightEntity->SetLight(this->currLightAnimKey.GetValue());

        this->lightEntity->SetTransform(this->ComputeWorldSpaceTransform());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
LightEffect::ClearLights()
{
    this->lights.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
LightEffect::SetLights(const nArray<nTime>& times, const nArray<nLight>& lights)
{
    n_assert(times.Size() == lights.Size());

    this->lights.Clear();

    int i, num = times.Size();
    for (i = 0; i < num; i++)
    {
        this->AddLight(times[i], lights[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
LightEffect::AddLight(const nTime& time, const nLight& light)
{
    lights.InsertSorted(nAnimKey<nLight>((float)time, light));
}

//------------------------------------------------------------------------------
/**
*/
void 
LightEffect::SetLight(const nLight& light)
{
    this->lights.Clear();
    this->AddLight(0.0f, light);
}

}; // namespace VFX
