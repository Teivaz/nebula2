//------------------------------------------------------------------------------
//  shadowdepth_tree.fx
//
//  Render depth values to shadow map.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the modelview*projection matrix of the light source
float3x3 Swing;                         // the swing rotation matrix
float3 BoxMinPos;                       // model space bounding box min
float3 BoxMaxPos;                       // model space bounding box max

texture DiffMap0;

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;    
    float depth : TEXCOORD1;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  the vertex shader
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // compute lerp factor based on height above min y
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);

    // compute rotated vertex position, normal and tangent in model space
    float4 rotPosition = float4(mul(Swing, vsIn.position), 1.0f);

    // lerp between original and rotated pos
    float4 lerpPosition = lerp(vsIn.position, rotPosition, ipol);

    // get clip space coordinate
    float4 pos = mul(lerpPosition, ModelViewProjection);
    vsOut.position = pos;

    // pass z / w to the pixel shader
    vsOut.uv0 = vsIn.uv0;
    vsOut.depth = (pos.z + 0.0001f) / pos.w;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader writes normalized depth values to the shadow map.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    float alpha = tex2D(DiffSampler, psIn.uv0).a - 0.5f;
    clip(alpha);
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
