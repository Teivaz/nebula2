#line 1 "default.fx"
//------------------------------------------------------------------------------
//  ps2.0/default.fx
//
//  The default shader for dx9 cards using vertex/pixel shader 2.0.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   ModelLightPos;

shared float4 LightDiffuse;         // light diffuse color        
shared float4 LightSpecular;        // light specular color
shared float4 LightAmbient;         // light ambient color
shared float4 LightDiffuse1;        // light diffuse color        

float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float4 MatAmbient;                  // material ambient color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture BumpMap0;                   // 2d bump texture

int  AlphaRef = 100;
int  CullMode = 2;                   // CW

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;  
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT; 
};

struct VsOutput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 primLightVec : TEXCOORD1;
    float3 primHalfVec  : TEXCOORD2;
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
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsLighting(vsIn.position, vsIn.normal, vsIn.tangent,
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
    float4 diffuse     = MatDiffuse  * LightDiffuse;
    float4 specular    = MatSpecular * LightSpecular;
    float4 ambient     = MatAmbient  * LightAmbient;
    float4 secDiffuse  = MatDiffuse  * LightDiffuse1;
    
    float4 diffColor = tex2D(DiffSampler, psIn.uv0);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0).rgb * 2.0f) - 1.0f;
    
    float4 baseColor = psLighting2(diffColor, tangentSurfaceNormal, psIn.primLightVec, psIn.primHalfVec,
                                   diffuse, specular, ambient, secDiffuse, MatSpecularPower, 1.0f);
    return baseColor;
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode     = <CullMode>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
