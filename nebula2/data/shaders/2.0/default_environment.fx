//------------------------------------------------------------------------------
//  ps2.0/default_environment.fx
//
//  A spherical environment shader.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   ModelLightPos;
shared float4x4 Model;

shared float4 LightDiffuse;         // light diffuse color        
shared float4 LightSpecular;        // light specular color
shared float4 LightAmbient;         // light ambient color
shared float4 LightDiffuse1;        // light diffuse color        

float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float4 MatAmbient;                  // material ambient color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture CubeMap0;                   // 2d environment texture (not a cube map)
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
    float3 worldReflect : TEXCOORD3;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/bumpsampler.fx"
sampler EnvironmentSampler = sampler_state
{
    Texture = <CubeMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    
    // compute per-vertex-parameters for pixel lighting
    vsLighting(vsIn.position, vsIn.normal, vsIn.tangent,
               ModelLightPos, ModelEyePos,
               vsOut.primLightVec, vsOut.primHalfVec);
               
    // compute a model space reflection vector
    float3 modelEyeVec = normalize(vsIn.position.xyz - ModelEyePos);
    float3 modelReflect = reflect(modelEyeVec, vsIn.normal);
    
    // transform model space reflection vector to view space
    vsOut.worldReflect = mul(modelReflect, (float3x3)Model);

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
    
    float4 reflectColor = tex2D(EnvironmentSampler, psIn.worldReflect);
    float4 color = lerp(diffColor, reflectColor, diffColor.a);
    float specModulate = diffColor.a;
    float4 finalColor = psLighting2(color, tangentSurfaceNormal, psIn.primLightVec, psIn.primHalfVec,
                                    diffuse, specular, ambient, secDiffuse, MatSpecularPower, specModulate);
    return finalColor;
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
