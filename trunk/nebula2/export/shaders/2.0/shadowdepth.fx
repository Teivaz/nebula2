//------------------------------------------------------------------------------
//  shadowdepth.fx
//
//  Render depth values to shadow map.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;   // the modelview*projection matrix of the light source

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;    
};

struct VsOutput
{
    float4 position : POSITION;
    float depth : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  the vertex shader
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 pos = mul(vsIn.position, ModelViewProjection);
    vsOut.position = pos;

    // pass z / w to the pixel shader
    vsOut.depth = (pos.z + 0.0001f) / pos.w;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader writes normalized depth values to the shadow map.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return psIn.depth;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = true;
        ColorWriteEnable = RED | GREEN | BLUE | ALPHA;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = Ccw;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

//------------------------------------------------------------------------------
