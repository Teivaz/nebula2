#line 1 "diffsampler.fx"
//------------------------------------------------------------------------------
//  diffsampler.h
//  Declare the standard diffuse sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};
