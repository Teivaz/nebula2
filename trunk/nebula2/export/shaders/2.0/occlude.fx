//------------------------------------------------------------------------------
//  occlude.fx
//
//  Initialize the depth buffer.
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
};

//------------------------------------------------------------------------------
//  the vertex shader
//------------------------------------------------------------------------------
float4 vsMain(const VsInput vsIn) : POSITION
{
    return mul(vsIn.position, ModelViewProjection);
}

//------------------------------------------------------------------------------
//  The pixel shader writes normalized depth values to the shadow map.
//------------------------------------------------------------------------------
float4 psMain() : COLOR
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);;
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
        CullMode         = Cw;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

//------------------------------------------------------------------------------
