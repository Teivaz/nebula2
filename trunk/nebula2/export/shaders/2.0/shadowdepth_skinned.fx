#line 1 "shadowdepth_skinned.fx"
//------------------------------------------------------------------------------
//  shadowdepth_skinned.fx
//
//  Render depth values to shadow map.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the modelview*projection matrix of the light source
matrix<float,4,3> JointPalette[72];     // the joint palette for skinning

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES; 
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

    // get skinned position, normal and tangent    
    float4 skinPos = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    float4 pos = mul(skinPos, ModelViewProjection);
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
