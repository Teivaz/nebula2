//------------------------------------------------------------------------------
//  shadowdepth_tree.fx
//
//  Render depth values to shadow map.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 InvModelView;              // inverse of model view
shared float4x4 ModelViewProjection;       // the model*view*projection matrix

texture DiffMap0;

float Time;                         // the current time
float4 Wind;                        // wind direction, strength in .w
float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max
float3 BoxCenter;                   // model space bounding box center

float SpriteSwingTime;              // swing period
float SpriteSwingAngle;             // swing angle modulator
float InnerLightIntensity;          // light intensity at center
float OuterLightIntensity;          // light intensity at periphery

float4 GenAngle = { 3.54824948311, -11.6819286346, 10.4263944626, -2.29271507263 };

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 extrude  : NORMAL;           // sprite extrusion vector
    float2 uv0 :      TEXCOORD0;
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
    
    // compute swayed position in model space (see tree.fx for details)
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);
    float4 rotPosition  = float4(mul(Swing, vsIn.position), 1.0f);
    float4 position = lerp(vsIn.position, rotPosition, ipol);
        
    // compute rotation angle
    float t = frac((Time / SpriteSwingTime) + vsIn.position.x);
    float t2 = t * t;
    float t3 = t2 * t;
    float ang = SpriteSwingAngle * t * (GenAngle.x * t3 + GenAngle.y * t2 + GenAngle.z * t + GenAngle.w);

    // build rotation matrix
    float sinAng, cosAng;
    sincos(ang, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    // rotate extrude vector and transform to model space
    float3 extrudeVec = mul(rot, vsIn.extrude);
    float3 modelExtrudeVec = mul(extrudeVec, (float3x3)InvModelView);
        
    // extrude to position and transform to 
    position.xyz += modelExtrudeVec.xyz;
    
    // get clip space coordinate
    float4 pos = mul(position, ModelViewProjection);
    vsOut.position = pos;

    // pass z / w to the pixel shader
    vsOut.depth = (pos.z + 0.0001f) / pos.w;
    vsOut.uv0   = vsIn.uv0;
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
        CullMode         = None;
 
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

//------------------------------------------------------------------------------
