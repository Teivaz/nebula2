#line 1 "shadowmodsampler.fx"
//------------------------------------------------------------------------------
//  shadowmodsampler.h
//  Declare the shadow modulation sampler.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler ShadowModSampler = sampler_state
{
    Texture = <ShadowModMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

