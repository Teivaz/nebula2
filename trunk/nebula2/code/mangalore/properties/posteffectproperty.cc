//------------------------------------------------------------------------------
//  properties/posteffectproperty.h
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/posteffectproperty.h"
#include "variable/nvariableserver.h"
#include "game/entity.h"

namespace Attr
{
    DefineFloat(Saturation);
    DefineVector4(Luminance);
    DefineVector4(Balance);
    DefineFloat(BrightPassThreshold);
    DefineFloat(BrightPassOffset);
    DefineFloat(BloomScale);
    DefineVector4(FogColor);
    DefineFloat(FogNearDist);
    DefineFloat(FogFarDist);
    DefineFloat(FocusDist);
    DefineFloat(FocusLength);
    DefineFloat(NoiseIntensity);
    DefineFloat(NoiseScale);
    DefineFloat(NoiseFrequency);
};

namespace Properties
{
ImplementRtti(Properties::PostEffectProperty, Game::Property);
ImplementFactory(Properties::PostEffectProperty);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
PostEffectProperty::PostEffectProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PostEffectProperty::~PostEffectProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
    Makes only sense for live entities.
*/
int
PostEffectProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    GetEntity()->SetFloat(Attr::Saturation, 1.0f);
    GetEntity()->SetVector4(Attr::Luminance, vector4(0.299f, 0.587f, 0.114f, 0.0f));
    GetEntity()->SetVector4(Attr::Balance, vector4(1.3f, 1.092f, 0.923f, 0.0f));
    GetEntity()->SetFloat(Attr::BrightPassThreshold, 0.2f);
    GetEntity()->SetFloat(Attr::BrightPassOffset, 2.2f);
    GetEntity()->SetFloat(Attr::BloomScale, 0.3f);
    GetEntity()->SetVector4(Attr::FogColor, vector4(0.065f, 0.104f, 0.12f, 1.0f));
    GetEntity()->SetFloat(Attr::FogNearDist, 0.0f);
    GetEntity()->SetFloat(Attr::FogFarDist, 200.0f);
    GetEntity()->SetFloat(Attr::FocusDist, 0.0f);
    GetEntity()->SetFloat(Attr::FocusLength, 180.0f);
    GetEntity()->SetFloat(Attr::NoiseIntensity, 0.02f);
    GetEntity()->SetFloat(Attr::NoiseScale, 11.86f);
    GetEntity()->SetFloat(Attr::NoiseFrequency, 20.22f);
}

//------------------------------------------------------------------------------
/**
    Apply the post-effect rendering attributes to Nebula2.
*/
void
PostEffectProperty::OnRender()
{
    // update global Nebula2 display variables
    nVariableServer* varServer = nVariableServer::Instance();
    varServer->SetFloatVariable("Saturation", GetEntity()->GetFloat(Attr::Saturation));
    varServer->SetVectorVariable("Luminance", GetEntity()->GetVector4(Attr::Luminance));
    varServer->SetVectorVariable("Balance", GetEntity()->GetVector4(Attr::Balance));
    varServer->SetFloatVariable("HdrBrightPassThreshold", GetEntity()->GetFloat(Attr::BrightPassThreshold));
    varServer->SetFloatVariable("HdrBrightPassOffset", GetEntity()->GetFloat(Attr::BrightPassOffset));
    varServer->SetFloatVariable("HdrBloomScale", GetEntity()->GetFloat(Attr::BloomScale));
    varServer->SetVectorVariable("FogColor", GetEntity()->GetVector4(Attr::FogColor));
    vector4 fogAttrs;
    fogAttrs.x = GetEntity()->GetFloat(Attr::FogNearDist);
    fogAttrs.y = GetEntity()->GetFloat(Attr::FogFarDist);
    varServer->SetVectorVariable("FogDistances", fogAttrs);
    vector4 focusAttrs;
    focusAttrs.x = GetEntity()->GetFloat(Attr::FocusDist);
    focusAttrs.y = GetEntity()->GetFloat(Attr::FocusLength);
    varServer->SetVectorVariable("CameraFocus", focusAttrs);
    varServer->SetFloatVariable("NoiseIntensity", GetEntity()->GetFloat(Attr::NoiseIntensity));
    varServer->SetFloatVariable("NoiseScale", GetEntity()->GetFloat(Attr::NoiseScale));
    varServer->SetFloatVariable("NoiseFrequency", GetEntity()->GetFloat(Attr::NoiseFrequency));
}

}; // namespace Properties
