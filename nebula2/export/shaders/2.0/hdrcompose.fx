//------------------------------------------------------------------------------
//  hdrcompose.fx
//
//  Frame composition shader for the HDR renderer.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

texture DiffMap0;
texture DiffMap1;
float4 SampleOffsets[15];
float4 SampleWeights[15];
float4 MatDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // tint color

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct vsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct vsOutput
{
    float4 position  : POSITION;
    float2 uv0 : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler HdrBuffer = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

sampler PostEffectBuffer = sampler_state
{
    Texture = <DiffMap1>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
vsOutput vsMain(const vsInput vsIn)
{
    vsOutput vsOut;
    vsOut.position = vsIn.position;
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
float4 psMain(const vsOutput psIn) : COLOR
{
    float4 imageColor = tex2D(HdrBuffer, psIn.uv0);
    int i;
    float4 peColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (i = 0; i < 15; i++)
    {
        peColor += SampleWeights[i] * tex2D(PostEffectBuffer, psIn.uv0 + SampleOffsets[i].xy);
    }    
    float4 color = (imageColor + peColor) * MatDiffuse;
    return color;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        // ZFunc         = LessEqual;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = True;
        AlphaTestEnable  = False;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        CullMode         = None;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
