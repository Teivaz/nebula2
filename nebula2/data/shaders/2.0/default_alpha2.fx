//------------------------------------------------------------------------------
//  fixed/default_alpha2.fx
//
//  A 2 layer alpha shader. Note: the alpha channel of the first 
//  texture indicates the transparency of the 2nd texture!
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 Model;

shared float4 LightAmbient;
shared float4 LightDiffuse;
shared float4 LightSpecular;

shared float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };
shared float4x4 TextureTransform1 = {1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };

shared float3 LightPos;    // the light's position in world space

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;

int AlphaRef = 100;

int AlphaSrcBlend = 5;              // SrcAlpha
int AlphaDstBlend = 6;              // InvSrcAlpha

texture DiffMap0;
texture DiffMap1;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/diff1sampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
    	TextureTransform[0] = <TextureTransform0>;
    	TextureTransform[1] = <TextureTransform1>;

        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;

        VertexShader = 0;
        PixelShader  = 0;

        MaterialDiffuse  = <MatDiffuse>;
        MaterialAmbient  = <MatAmbient>;
        MaterialSpecular = <MatSpecular>;
        MaterialPower    = <MatSpecularPower>;

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

        FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransformFlags[1] = Count2;
   
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <Diff1Sampler>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;

        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        
        ColorOp[2]   = Modulate;
        ColorArg1[2] = Diffuse;        
        ColorArg2[2] = Current;
        
        ColorOp[3]   = Disable;
    }
}

