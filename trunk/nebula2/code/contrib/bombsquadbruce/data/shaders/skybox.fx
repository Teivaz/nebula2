#line 1 "skybox.fx"
//------------------------------------------------------------------------------
//  skybox.fx
//
//  The skybox shader for DX8 cards
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;
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
        WorldTransform[0] = <Model>;
        ViewTransform = <View>;
        ProjectionTransform = <Projection>;
        TextureTransform[0] = <TextureTransform0>;

        ZWriteEnable = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        
        AlphaBlendEnable = False;    
        AlphaTestEnable  = False;

        CullMode = 2;

        VertexShader = 0;
        PixelShader  = 0;

        FogEnable = False;
        Lighting = False;

        Sampler[0] = <DiffSampler>;    
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0]    = Disable;
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
     
        ColorOp[1]   = Disable;
    }
}

