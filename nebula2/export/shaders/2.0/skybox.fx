#line 1 "skybox.fx"
//------------------------------------------------------------------------------
//  fixed/skybox.fx
//
//  A simple shader for the skybox.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;

float3 SunPos = float3(-100000, 2000, -26800);

texture DiffMap0;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

struct VsInput
{
    float4 position  : POSITION;  // the particle position in world space
    float2 uv0       : TEXCOORD0; // the particle texture coordinates
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------

VsOutput vsMain(const VsInput vsIn)
{	
    VsOutput vsOut;
    // transform to projection space
    vsOut.position      = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0           = vsIn.uv0;
    
    vsSky(vsIn.position, SunPos, vsOut.diffuse);

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{	
    float4 color = psIn.diffuse;
    float3 texCol = tex2D(DiffSampler, psIn.uv0);
    return float4(lerp(texCol, color, color.a), 1.0);
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;

        //FillMode = WireFrame;

        CullMode         = None;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
