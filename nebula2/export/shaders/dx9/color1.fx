//------------------------------------------------------------------------------
//  color1.fx
//    
//  Render unlit pixel color with 1 texture layer.
//------------------------------------------------------------------------------
#include "lib.fx"

float4x4 modelView;             // the modelview matrix
float4x4 modelViewProjection;   // the modelview*projection matrix
float4 modelEyePos;             // eye position in model space
float4 modelLightPos;           // light position in model space

texture diffMap;                // the diffuse color texture map

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position  : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler DiffuseMap = sampler_state
{
    Texture = <diffMap>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    vsOut.position = transformStatic(vsIn.position, modelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
PS_OUTPUT psMain(const VS_OUTPUT psIn)
{
    PS_OUTPUT psOut;
    psOut.color = tex2D(DiffuseMap, psIn.uv0);
    return psOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1, 1 Texture Layer
//------------------------------------------------------------------------------
technique vs11_ps11
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend         = Zero;
        DestBlend        = SrcColor;
        
        Sampler[0] = <DiffuseMap>;
        
        VertexShader = compile vs_1_1 vsMain();
        PixelShader = compile ps_1_1 psMain();
    }
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, Fixed Function Pixel Pipeline
//------------------------------------------------------------------------------
technique vs11_ffpp
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = True;
        SrcBlend         = Zero;
        DestBlend        = SrcColor;
        
        Sampler[0] = <DiffuseMap>;

        VertexShader = compile vs_1_1 vsMain();
        
        PixelShader = 0;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        
        AlphaOp[0] = Disable;
        ColorOp[1] = Disable;
    }
}
