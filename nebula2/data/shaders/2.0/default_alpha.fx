//------------------------------------------------------------------------------
//  ps2.0/default_alpha.fx
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the model*view*projection matrix
shared float3   ModelEyePos;            // the eye position in model space
shared float3   ModelLightPos;          // the light position in model space

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

int  CullMode = 2;                  // CW

int AlphaSrcBlend = 5;              // SrcAlpha
int AlphaDstBlend = 6;              // InvSrcAlpha

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
    float4 diffuse  = MatDiffuse * LightDiffuse;
    float4 specular = MatSpecular * LightSpecular;
    float4 ambient  = MatAmbient * LightAmbient;               
    float4 secDiffuse  = MatDiffuse  * LightDiffuse1;

    float4 diffColor = tex2D(DiffSampler, psIn.uv0);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0).rgb * 2.0f) - 1.0f;
    float specModulate = diffColor.a;

    float4 finalColor = psLightingAlpha(diffColor, tangentSurfaceNormal, psIn.primLightVec, psIn.primHalfVec,
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
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
