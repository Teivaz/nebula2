//------------------------------------------------------------------------------
//  ps2.0/default_lightmapped.fx
//
//  A simple lightmapped shader.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   ModelLightPos;

shared float4 LightDiffuse; 
shared float4 LightSpecular;
shared float4 LightAmbient; 

float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float4 MatAmbient;                  // material ambient color
float  MatSpecularPower;            // the material's specular power

float Intensity0 = 2.0f;            // hdr multiplier

texture DiffMap0;
texture BumpMap0;
texture DiffMap1;

int AlphaRef = 100;
int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
    float2 uv1 :      TEXCOORD1;
    float3 tangent :  TANGENT; 
};

struct VsOutput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float2 uv1          : TEXCOORD1;
    float3 primLightVec : TEXCOORD2;
    float3 primHalfVec  : TEXCOORD3;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/lightmapsampler.fx"
#include "../lib/bumpsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv1 = vsIn.uv1;

    vsLighting(vsIn.position, vsIn.normal, vsIn.tangent,
               vsIn.position + vsIn.normal, ModelEyePos,
               vsOut.primLightVec, vsOut.primHalfVec);

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    float4 diffuse  = MatDiffuse * LightDiffuse;
    float4 specular = MatSpecular * LightSpecular;
    float4 ambient  = MatAmbient * LightAmbient;
        
    float4 baseColor = psLighting(DiffSampler, BumpSampler,
                                  psIn.uv0, psIn.primLightVec, psIn.primHalfVec,
                                  diffuse, specular, ambient, MatSpecularPower);

    float4 lightmapColor = tex2D(LightmapSampler, psIn.uv1);
    float4 color = baseColor * lightmapColor * Intensity0;
    return color;
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

                