//------------------------------------------------------------------------------
//  fixed/skybox.fx
//
//  A simple shader for the skybox.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;
float4x4 ModelView;
float4x4 Model;
float3 ModelEyePos;
float3 ModelLightPos;
float4x4 View;
float4x4 Projection;

texture DiffMap0;

int CullMode = 2;                   // default value (CW); must not be 0 for software vp

struct VsInput
{
    float4 position  : POSITION;  // the particle position in world space
    float2 uv0       : TEXCOORD0; // the particle texture coordinates
    float4 color     : COLOR0;    // the particle color
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
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------

VsOutput vsMain(const VsInput vsIn)
{	
    VsOutput vsOut;
    // transform to projection space
    vsOut.position      = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0           = vsIn.uv0;
    
    vsSky(vsIn.position, ModelLightPos, vsOut.diffuse);

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
        WorldTransform[0]	= <Model>;
        ViewTransform		= <View>;
        ProjectionTransform = <Projection>;

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
