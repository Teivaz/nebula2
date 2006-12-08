#line 1 "bump2sampler.fx"
//------------------------------------------------------------------------------
//  bump2sampler.fx
//  Declare the standard bump sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler Bump2Sampler = sampler_state
{
    Texture   = <BumpMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
