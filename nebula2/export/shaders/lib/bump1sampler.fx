#line 1 "bump1sampler.fx"
//------------------------------------------------------------------------------
//  bump1sampler.fx
//  Declare the standard bump sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler Bump1Sampler = sampler_state
{
    Texture   = <BumpMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
