//------------------------------------------------------------------------------
//  compose.fx
//
//  Frame compositing shader.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

texture DiffMap0;
float Intensity0;
float4 MatDiffuse;      // the color balance value
float4 MatAmbient = {-0.299, -0.587, -0.114, 1.0};  // the b&w conversion value (usually x=0.299, y=0.587, z=0.114, w=1.0)

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
sampler RenderBuffer = sampler_state
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
//  the pixel shader function
//------------------------------------------------------------------------------
float4 psMain(const vsOutput psIn) : COLOR
{
    float4 imageColor = tex2D(RenderBuffer, psIn.uv0);
    float luminance = dot(imageColor.xyz, MatAmbient.xyz);
    return MatDiffuse * lerp(float4(luminance, luminance, luminance, luminance), imageColor, Intensity0);
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
