#line 1 "default_skinned.fx"
//------------------------------------------------------------------------------
//  fixed/default_skinned.fx
//
//  The default skinning shader for dx7 cards.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

uniform float4x4 ModelViewProjection;       // the model*view*projection matrix
uniform float4x4 ModelLightProjection;      // the model*light*projection matrix
uniform float3   ModelLightPos;             // the light's position in model space
matrix<float,4,3> JointPalette[72];        // the joint palette for skinning

float4 MatAmbient;
float4 MatDiffuse;

int AlphaRef = 100;
int CullMode = 2;       // CW

texture DiffMap0;

uniform float4 LightAmbient;                // light's ambient component
uniform float4 LightDiffuse;                // light's diffuse component

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES; 
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
#include "shaders:../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // get skinned position, normal and tangent    
    float3 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
    
    // transform vertex position
    vsOut.position = transformStatic(skinPos, ModelViewProjection);
    
    // compute lighting
    float3 N = skinNormal;
    float3 P = skinPos;
    float3 L = normalize(ModelLightPos - P);
    
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = LightAmbient * MatAmbient + MatDiffuse * LightDiffuse * max(0, dot(N, L));

    return vsOut;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        TextureTransformFlags[0] = 0;
        AlphaRef = <AlphaRef>;
        CullMode = <CullMode>;
        
        VertexShader = compile vs_2_0 vsMain();
        
        // Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}
