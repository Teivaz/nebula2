#line 1 "shadowmapsampler.fx"
//------------------------------------------------------------------------------
//  shadowmapsampler.h
//  Declare the shadow map sampler.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};
