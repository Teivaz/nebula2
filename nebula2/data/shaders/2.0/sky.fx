#line 1 "sky.fx"
//------------------------------------------------------------------------------
//  ps2.0/sky.fx
//
//  A simple gradient sky shader.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;
shared float4 FogColor;
shared float4 FogParams;

float Intensity0;
float4 MatDiffuse;                  // material diffuse color
texture DiffMap0;                   // 2d texture

#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;        // texture coordinate
    float  fog : TEXCOORD1;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.fog = vsExpFog(vsIn.position * 20000, vsIn.position * 5000, float3(0.0f, 0.0f, 0.0f), FogParams);        
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    float4 color = tex2D(DiffSampler, psIn.uv0) * MatDiffuse * Intensity0;
    return lerp(color, FogColor, psIn.fog);
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode = NONE;
        AlphaRef = 0;
        ZWriteEnable = false;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
