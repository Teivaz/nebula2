#line 1 "lightmapsampler.fx"
//------------------------------------------------------------------------------
//  lightmapsampler.h
//  Declare the standard light map sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler LightmapSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};
