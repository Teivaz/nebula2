#line 1 "skybox.fx"
//------------------------------------------------------------------------------
//  skybox.fx
//
//  The skybox shader for DX8 cards
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

texture DiffMap0;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "diffsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        TextureTransform[0] = <TextureTransform0>;
        CullMode = Cw;

        Sampler[0] = <DiffSampler>;    
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0]    = Disable;
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
     
        ColorOp[1]   = Disable;
    }
}

