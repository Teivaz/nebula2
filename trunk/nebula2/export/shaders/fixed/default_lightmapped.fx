#line 1 "default_lightmapped.fx"
//------------------------------------------------------------------------------
//  fixed/default_lightmapped.fx
//
//  The default shader for DX7 cards
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
float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;

int AlphaRef = 10;

texture DiffMap0;
texture DiffMap1;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/lightmapsampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        TextureTransform[0] = <TextureTransform0>;
        TextureTransform[1] = <TextureTransform1>;
        
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;

        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;

        CullMode         = <CullMode>;
 
        FogEnable = true;
        FogColor = {0.5, 0.5, 0.5, 1.0};
        FogVertexMode = None;
        FogTableMode = Exp2;
        FogDensity = 5.0e-4;
  	
        Lighting = false;
	    
        FVF = XYZ | NORMAL | TEX2;
        
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0]    = Count2;

        TexCoordIndex[1] = 1;
        TextureTransformFlags[1]    = Count2;
        
        VertexShader = 0;
        PixelShader  = 0;
        
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <LightmapSampler>;
        
        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = 0;
               
        ColorOp[1] = Disable;
        
        ColorOp[1] = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        ColorOp[2] = Disable;
    }
}

