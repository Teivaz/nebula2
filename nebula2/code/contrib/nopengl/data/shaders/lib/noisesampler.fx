#line 1 "noisesampler.fx"
//------------------------------------------------------------------------------
//  noisesampler.h
//  Declare a noise sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler NoiseSampler = sampler_state
{
    Texture = <NoiseMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

