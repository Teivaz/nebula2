//------------------------------------------------------------------------------
//  ps2.0/gradientsky.fx
//
//  A simple gradient sky shader.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;       // the model*view*projection matrix

int AlphaRef = 100;
int CullMode = 2;

float4 HorizonColor = { 142.0 / 255.0, 140.0 / 255.0, 153.0 / 255.0, 1.0 };
float4 ZenitColor   = { 45.0 / 255.0, 62.0 / 255.0, 82.0 / 255.0, 1.0 };

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float4 color        : COLOR0;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    float height = saturate(vsIn.position.y / 50.0f);
    vsOut.color = lerp(HorizonColor, ZenitColor, height);
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return psIn.color;
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = true;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = False;
        ZFunc            = Always;
        AlphaBlendEnable = False;
        CullMode         = None;
        AlphaTestEnable  = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
