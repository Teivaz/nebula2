//------------------------------------------------------------------------------
//  ps2.0/default_skinned.fx
//
//  The default skinning shader for dx9 cards using vertex/pixel shader 2.0,
//  running in 1 render pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the model*view*projection matrix
shared float3   ModelLightPos;          // the light's position in model space
shared float3   ModelEyePos;            // the eye position in model space
matrix<float,4,3> JointPalette[72];     // the joint palette for skinning

shared float4 LightDiffuse;             // light diffuse color        
shared float4 LightSpecular;            // light specular color
shared float4 LightAmbient;             // light ambient color
shared float4 LightDiffuse1;        // light diffuse color        

float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float4 MatAmbient;                  // material ambient color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture BumpMap0;                   // 2d texture

int AlphaRef = 100;
int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float3 tangent :  TANGENT; 
    float2 uv0 :      TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES; 
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float2 uv0          : TEXCOORD0;        // texture coordinate
    float3 primLightVec : TEXCOORD1;        // primary light vector
    float3 primHalfVec  : TEXCOORD2;        // primary half vector
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/bumpsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // get skinned position, normal and tangent    
    float4 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    vsOut.position     = mul(skinPos, ModelViewProjection);
    vsOut.uv0          = vsIn.uv0;
    float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinTangent = skinnedNormal(vsIn.tangent, vsIn.weights, vsIn.indices, JointPalette);
    
    // compute lighting
    vsLighting(skinPos, skinNormal, skinTangent,
               ModelLightPos, ModelEyePos,
               vsOut.primLightVec, vsOut.primHalfVec);
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // compute lit color
    float4 diffuse  = MatDiffuse * LightDiffuse;
    float4 specular = MatSpecular * LightSpecular;
    float4 ambient  = MatAmbient * LightAmbient;
    float4 secDiffuse = MatDiffuse * LightDiffuse1;
    
    float4 diffColor = tex2D(DiffSampler, psIn.uv0);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0).rgb * 2.0f) - 1.0f;
    
    float4 color = psLighting2(diffColor, tangentSurfaceNormal, psIn.primLightVec, psIn.primHalfVec,
                               diffuse, specular, ambient, secDiffuse, MatSpecularPower, 1.0f);
    return color;                                 
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode = <CullMode>;        
        AlphaRef = <AlphaRef>;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

                