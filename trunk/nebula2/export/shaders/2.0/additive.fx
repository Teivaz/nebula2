//------------------------------------------------------------------------------
//  ps2.0/additive.fx
//
//  A simple unlit, additive transparency shader.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;       // the model*view*projection matrix
float4 MatDiffuse;                  // material diffuse color
texture DiffMap0;                   // 2d texture
int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float2 uv0 :      TEXCOORD0;
};

struct VsOutput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return tex2D(DiffSampler, psIn.uv0) * MatDiffuse;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = false;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend		 = One;
        DestBlend		 = One;
        CullMode         = <CullMode>;
        
        AlphaTestEnable  = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
