//------------------------------------------------------------------------------
//  ps2.0/default_alpha.fx
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;    // the model*view*projection matrix
shared float3   ModelEyePos;            // the eye position in model space
shared float3   ModelLightPos;          // the light position in model space

float4 LightDiffuse;                // light diffuse color        
float4 LightSpecular;               // light specular color
float4 LightAmbient;                // light ambient color
float4 MatDiffuse;                  // material diffuse color
float4 MatSpecular;                 // material specular color
float  MatSpecularPower;            // the material's specular power

texture DiffMap0;                   // 2d texture
texture BumpMap0;                   // 2d texture

int  CullMode = 2;                  // CW

int AlphaSrcBlend = 5;              // SrcAlpha
int AlphaDstBlend = 6;              // InvSrcAlpha

static bool EnableShadows = true;

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
    // float3 F_ex         : TEXCOORD3;        // light outscatter coefficient
    // float3 L_in         : TEXCOORD4;        // light inscatter coefficient
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
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsLighting(vsIn.position, vsIn.normal, vsIn.tangent,
               ModelLightPos, ModelEyePos,
               vsOut.primLightVec, vsOut.primHalfVec);
               
    // vsAthmoFog(vsIn.position, ModelEyePos, ModelLightPos, vsOut.L_in, vsOut.F_ex);
    
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
    float4 baseColor = psLighting(DiffSampler, BumpSampler,
                                  psIn.uv0, psIn.primLightVec, psIn.primHalfVec,
                                  diffuse, specular, LightAmbient, MatSpecularPower);
    // return psAthmoFog(psIn.L_in, psIn.F_ex, baseColor);
    return baseColor;
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = false;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;       
        ZEnable          = true;
        ZFunc            = LessEqual;
        CullMode         = <CullMode>;

        AlphaBlendEnable = true;
        SrcBlend         = <AlphaSrcBlend>;
        DestBlend        = <AlphaDstBlend>;
        
        AlphaTestEnable  = false;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
