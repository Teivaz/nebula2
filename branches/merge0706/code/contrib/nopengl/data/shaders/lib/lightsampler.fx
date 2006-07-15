#line 1 "lightsampler.fx"
//------------------------------------------------------------------------------
//  lightsampler.h
//  Declare the standard light modulation sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler LightSampler = sampler_state
{
    Texture     = <LightModMap>;
//    AddressU    = Clamp;
//    AddressV    = Clamp;
    AddressU    = Wrap;
    AddressV    = Wrap;
    MinFilter   = Linear;
    MagFilter   = Linear;
    MipFilter   = None;             // IMPORTANT for proper Z-Clamping!!!
};
