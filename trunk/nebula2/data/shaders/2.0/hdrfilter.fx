//------------------------------------------------------------------------------
//  hdrfilter.fx
//
//  Post effect shader for the hdr renderer. Applies a filter kernel.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

texture DiffMap0;
float4 SampleOffsets[15];
float4 SampleWeights[15];

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
sampler SourceSampler = sampler_state
{
    Texture = <DiffMap0>;
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
//  Pixel shader to apply a filter kernel with 15 samples.
//------------------------------------------------------------------------------
float4 psMain(const vsOutput psIn) : COLOR
{
    int i;
    float4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    for (i = 0; i < 15; i++)
    {
        color += SampleWeights[i] * tex2D(SourceSampler, psIn.uv0 + SampleOffsets[i].xy);
    }
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
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
        
