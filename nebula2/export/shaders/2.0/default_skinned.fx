//------------------------------------------------------------------------------
//  ps2.0/default_skinned.fx
//
//  The default skinning shader for dx9 cards using vertex/pixel shader 2.0,
//  running in 1 render pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the model*view*projection matrix
shared float3   ModelLightPos;          // the light's position in model space
shared float3   ModelEyePos;            // the eye position in model space
matrix<float,4,3> JointPalette[72];     // the joint palette for skinning

float4 LightDiffuse;                // light diffuse color        
float4 LightSpecular;               // light specular color
float4 LightAmbient;                // light ambient color
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
#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/bumpsampler.fx"

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
    float4 color = psLighting(DiffSampler, BumpSampler,
                              psIn.uv0, psIn.primLightVec, psIn.primHalfVec,
                              diffuse, specular, LightAmbient, MatSpecularPower);
    return color;                                 
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
        ZEnable          = True;
        ZFunc            = LessEqual;
        AlphaBlendEnable = False;
        CullMode         = <CullMode>;        

        AlphaTestEnable  = True;
        AlphaFunc        = GreaterEqual;
        AlphaRef         = <AlphaRef>;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

                