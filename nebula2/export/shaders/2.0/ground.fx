//------------------------------------------------------------------------------
//  ps2.0/default.fx
//
//  A ground shader which mixes 2 textures based on ground angle.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;       // the model*view*projection matrix
float4x4 ModelLightProjection;      // the model*light*projection matrix
float4   ModelLightPos;             // the light's position in model space
float4   ModelEyePos;               // the eye position in model space

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;

int AlphaRef = 10;

float Time;
float Height;

texture DiffMap0;                   // ground material 1
texture DiffMap2;                   // ground material 2
texture BumpMap0;
texture ShadowMap;
texture ShadowModMap;

float4 LightAmbient;                // light's ambient component
float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

texture NoiseMap0;                  // a noise map

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
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0      : TEXCOORD0;        // texture coordinate
    float3 lightVec : TEXCOORD1;        // light vector
    float3 halfVec  : TEXCOORD2;        // half vector
//    float4 lightPos : TEXCOORD3;        // vertex position in light projection space
    float4 shadowPos    : TEXCOORD3;        // vertex position in shadow map space    
    float4 diffuse  : TEXCOORD4;        // LightDiffuse * MatDiffuse
    float4 specular : COLOR0;           // LightSpecular * MatSpecular, alpha is material lerp
    float4 ambient  : COLOR1;           // LightAmbietn * MatAmbient
};

struct PsOutput
{
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/diff2sampler.fx"
#include "../lib/bumpsampler.fx"
#include "../lib/shadowmapsampler.fx"
#include "../lib/shadowmodsampler.fx"
#include "../lib/noisesampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // transform vertex position
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    
    // compute light and half vector
    tangentSpaceLightHalfVector(vsIn.position, 
                                ModelLightPos,
                                ModelEyePos,
                                vsIn.normal,
                                vsIn.tangent,
                                vsOut.lightVec,
                                vsOut.halfVec);
                                
    // compute vertex position in light space
    vsOut.shadowPos = mul(vsIn.position, ModelLightProjection);

    // compute the material lerp factor based on ground angle
    vsOut.diffuse  = LightDiffuse * MatDiffuse;
    vsOut.specular.rgb = LightSpecular * MatSpecular;
    vsOut.specular.a   = abs(vsIn.normal.y * Height);
    vsOut.ambient  = LightAmbient * MatAmbient;
    vsOut.uv0 = vsIn.uv0;
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
PsOutput psMain(const VsOutput psIn)
{
    PsOutput psOut;

    // get diffuse color
    float4 diffuseColor = lerp(tex2D(Diff2Sampler, psIn.uv0), tex2D(DiffSampler, psIn.uv0), psIn.specular.a);
    float4 bump = tex2D(BumpSampler, psIn.uv0);
    float3 surfNormal   = 2.0f * (bump.xyz - 0.5f);
    float specularScale = bump.a;

    // compute diffuse and specular intensity
    float diffIntensity = saturate(dot(surfNormal, normalize(psIn.lightVec)));
    float specIntensity = specularScale * pow(saturate(dot(surfNormal, normalize(psIn.halfVec))), MatSpecularPower);

    // get noise-permuted projected shadow position
    float4 shadowModulate = shadow(psIn.shadowPos, 0.0001f, NoiseSampler, ShadowMapSampler, ShadowModSampler);

    // compute resulting color components
    float4 diffuse  = diffuseColor * diffIntensity * psIn.diffuse;
    float4 specular = psIn.specular * specIntensity;
    float4 ambient  = psIn.ambient * diffuseColor;

    // compute final color
    psOut.color.rgb = ((diffuse + specular) * shadowModulate) + ambient;
    psOut.color.a = diffuseColor.a;
    return psOut;
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
