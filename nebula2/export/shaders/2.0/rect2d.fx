//------------------------------------------------------------------------------
//  rect2d.fx
//
//  A simple 2d rect shader for debug purposes.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;   // the modelview*projection matrix
texture DiffMap0;

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

struct psOutput
{
    float4 color : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler ColorMap = sampler_state
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
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
psOutput psMain(const vsOutput psIn)
{
    psOutput psOut;
    psOut.color = tex2D(ColorMap, psIn.uv0);
    return psOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1, 1 Texture Layer
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = True;
        ZFunc            = Always;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
