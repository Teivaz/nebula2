//------------------------------------------------------------------------------
//  fixed/skybox.fx
//
//  A simple shader for the skybox.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;
float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

texture DiffMap0;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]	= <Model>;
        ViewTransform		= <View>;
        ProjectionTransform = <Projection>;
        TextureTransform[0] = <TextureTransform0>;

        ZWriteEnable     = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;

        CullMode         = <CullMode>;

        VertexShader = 0;
        PixelShader  = 0;

        MaterialAmbient  = {1.0, 1.0, 1.0, 1.0};
        MaterialDiffuse  = {0.0, 0.0, 0.0, 0.0};
        MaterialSpecular = {0.0, 0.0, 0.0, 0.0};

        Lighting = true;
        LightEnable[0]   = true;
        LightPosition[0] = {0.0, 0.0, 0.0, 0.0};
        LightAmbient[0]  = {1.0, 1.0, 1.0 ,1.0};
        LightDiffuse[0]  = {0.0, 0.0, 0.0, 0.0};
        LightSpecular[0] = {0.0, 0.0, 0.0, 0.0};

        LightType[0]   = DIRECTIONAL;
        SpecularEnable = false;

        FogEnable = false;

        Sampler[0]		= <DiffSampler>;

        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Disable;
   
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1] = Disable;
        
        AlphaOp[0] = Disable;
    }
}
