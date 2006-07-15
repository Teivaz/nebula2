#line 1 "default_environment_alpha.fx"
//------------------------------------------------------------------------------
//  fixed/default_environment_alpha.fx
//
//  The default shader for DX7 cards
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
uniform float4x4 Model;
uniform float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;
int AlphaSrcBlend = 5;  // SrcAlpha
int AlphaDstBlend = 6;  // InvSrcAlpha
int CullMode = 2;       // CCW

texture DiffMap0;
texture CubeMap0;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
// #include "shaders:../lib/diffsampler.fx"

/* sampler EnvironmentSampler = sampler_state
{
    Texture = <CubeMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
}; */

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
    	TextureTransform[0] = <TextureTransform0>;

        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;        
        MaterialDiffuse     = <MatDiffuse>;
        MaterialAmbient     = <MatAmbient>;

        // FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;
                
        TexCoordIndex[1] = CameraSpaceReflectionVector;
        TextureTransformFlags[1] = Count3;
        
        // Sampler[0] = <DiffSampler>;
        // Sampler[1] = <EnvironmentSampler>;

        // Base Texture
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;        
        
        // Reflection Texture
        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = SelectArg1;
        AlphaArg1[1] = Current;
       
        // Lighting
        ColorOp[2]   = Modulate;
        ColorArg1[2] = Diffuse;
        ColorArg2[2] = Current;
        AlphaOp[2]   = SelectArg1;
        AlphaArg1[2] = Current;

        ColorOp[3]   = Disable;
        AlphaOp[3]   = Disable;
    }
}

