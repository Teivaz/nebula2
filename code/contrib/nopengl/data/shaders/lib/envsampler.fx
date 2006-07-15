#line 1 "envsampler.fx"
//------------------------------------------------------------------------------
//  envsampler.fx
//  An environment cube map sampler.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler EnvSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = None;
};
    
    
    
    
