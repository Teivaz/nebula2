#line 1 "default_layered.fx"
//------------------------------------------------------------------------------
//  fixed/default_layered.fx
//
//  The default shader for DX7 cards
//  
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 Model;
shared float4x4 View;
shared float4x4 Projection;

float4x4 TextureTransform0 = {20.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 20.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 20.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f, 
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };

float4 MatAmbient;
float4 MatDiffuse;

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

float3 LightPos;                  // the light's position in world space

texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
texture DiffMap3;
texture NoiseMap0;
texture NoiseMap1;
texture NoiseMap2;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler LayerSampler0 = sampler_state
{
    Texture = <NoiseMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DiffSampler0  = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler LayerSampler1 = sampler_state
{
    Texture = <NoiseMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DiffSampler1  = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler LayerSampler2 = sampler_state
{
    Texture = <NoiseMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DiffSampler2  = sampler_state
{
    Texture = <DiffMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler DiffSampler3  = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;

        CullMode         = <CullMode>;
 
        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = <MatAmbient>;

        Ambient = {0,0,0,0};

        Lighting = True;
        LightEnable[0] = True;	

        LightAmbient[0]  = <LightAmbient>;
        LightDiffuse[0]  = <LightDiffuse>;
        LightSpecular[0] = <LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;
        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        SpecularEnable	= false;

        FogEnable = true;
        FogColor = {0.5, 0.5, 0.6, 1.0};
        FogVertexMode = Linear;
        FogStart = 200;
        FogEnd   = 800;
	    
        FVF = XYZ | NORMAL | TEX2;
        
        VertexShader = 0;
        PixelShader  = 0;

        TexCoordIndex[0]            = 0;
        TextureTransformFlags[0]    = Count2;
        TextureTransform[0]         = <TextureTransform0>;
        Sampler[0]		            = <DiffSampler0>;
               
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;

        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable; 
    }
    pass p1
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend		 = SrcAlpha;
        DestBlend		 = InvSrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;

        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = <MatAmbient>;

        Ambient = {0,0,0,0};

        Lighting = True;
        LightEnable[0] = True;	

        LightAmbient[0]  = <LightAmbient>;
        LightDiffuse[0]  = <LightDiffuse>;
        LightSpecular[0] = <LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;
        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        SpecularEnable	= false;

        FogEnable = true;
        FogColor = {0.5, 0.5, 0.6, 1.0};
        FogVertexMode = Linear;
        FogStart = 200;
        FogEnd   = 800;
	    
        FVF = XYZ | NORMAL | TEX2;
        
        VertexShader = 0;
        PixelShader  = 0;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <TextureTransform0>;
        Sampler[0]               = <DiffSampler1>;
        
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <TextureTransform1>;
        Sampler[1]               = <LayerSampler0>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
       
        ColorOp[1]   = SelectArg1;
        ColorArg1[1] = Current;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2] = Disable;
        AlphaOp[2] = Disable; 
    }
    pass p2
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend		 = SrcAlpha;
        DestBlend		 = InvSrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;

        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = <MatAmbient>;

        Ambient = {0,0,0,0};

        Lighting = True;
        LightEnable[0] = True;	

        LightAmbient[0]  = <LightAmbient>;
        LightDiffuse[0]  = <LightDiffuse>;
        LightSpecular[0] = <LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;
        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        SpecularEnable	= false;

        FogEnable = true;
        FogColor = {0.5, 0.5, 0.6, 1.0};
        FogVertexMode = Linear;
        FogStart = 200;
        FogEnd   = 800;
	    
        FVF = XYZ | NORMAL | TEX2;
        
        VertexShader = 0;
        PixelShader  = 0;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <TextureTransform0>;
        Sampler[0]               = <DiffSampler2>;
        
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <TextureTransform1>;
        Sampler[1]               = <LayerSampler1>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
       
        ColorOp[1]   = SelectArg1;
        ColorArg1[1] = Current;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2] = Disable;
        AlphaOp[2] = Disable; 
    }
    pass p3
    {
        WorldTransform[0]   = <Model>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;
        
        ZWriteEnable     = True;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend		 = SrcAlpha;
        DestBlend		 = InvSrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;

        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = <MatAmbient>;

        Ambient = {0,0,0,0};

        Lighting = True;
        LightEnable[0] = True;	

        LightAmbient[0]  = <LightAmbient>;
        LightDiffuse[0]  = <LightDiffuse>;
        LightSpecular[0] = <LightSpecular>;
        LightPosition[0] = <LightPos>;
        LightRange[0]    = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;
        // LightType must be the last light state that is set!
        LightType[0] = POINT;

        SpecularEnable	= false;

        FogEnable = true;
        FogColor = {0.5, 0.5, 0.6, 1.0};
        FogVertexMode = Linear;
        FogStart = 200;
        FogEnd   = 800;
	    
        FVF = XYZ | NORMAL | TEX2;
        
        VertexShader = 0;
        PixelShader  = 0;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <TextureTransform0>;
        Sampler[0]               = <DiffSampler3>;
        
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <TextureTransform1>;
        Sampler[1]               = <LayerSampler2>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
       
        ColorOp[1]   = SelectArg1;
        ColorArg1[1] = Current;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2] = Disable;
        AlphaOp[2] = Disable; 
    }
}

