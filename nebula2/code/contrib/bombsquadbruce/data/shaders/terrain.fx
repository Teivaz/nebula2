#line 1 "terrain.fx"
//------------------------------------------------------------------------------
//  terrain.fx
//
//  The terrain shader for DX8 cards
//  
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "lib.fx"

shared float4x4 Model;
shared float4x4 ModelViewProjection;
texture DiffMap0; // main texture
float TexScale = 15.0f;
float MatTranslucency; // the SQUARE ROOT of this is the distance at which objects become completely transparent
shared float4 LightAmbient;   
shared float4 LightDiffuse;    
shared float3 LightPos; 
//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler WrapSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv0 :  TEXCOORD0;
    float2 uv1 : TEXCOORD1;     
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
StdVsOutput vsMain(const VsInput vsIn)
{
    StdVsOutput vsOut;
    vsOut.diffuse = lightDiffuse( vsIn.normal, Model, LightAmbient, LightDiffuse, LightPos );
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0.xy = vsIn.uv0.xy * TexScale;     // generate texture coordinates
    vsOut.fog = fog( vsOut.diffuse.a, vsOut.position, MatTranslucency );

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = True;
        ZWriteEnable     = True;        
        AlphaTestEnable  = False;
        VertexShader = compile vs_1_1 vsMain();
        FogEnable = True;
        FogColor = {0.6, 0.8, 1.0, 0.0};
      
        Sampler[0] = <WrapSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Diffuse;
        ColorArg2[0] = Texture;
        
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Diffuse;
        AlphaArg2[0] = Texture;        
 
        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;
    }
}

