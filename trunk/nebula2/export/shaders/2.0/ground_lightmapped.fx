#line 1 "ground_lightmapped.fx"
//------------------------------------------------------------------------------
//  ps2.0/ground_lightmapped.fx
//
//  The ground lightmapped shader.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"

float4x4 ModelViewProjection;       // the model*view*projection matrix
float4x4 ModelLightProjection;      // the model*light*projection matrix
float4x4 Model;                     // the model matrix
float4   ModelLightPos;             // the light's position in model space
float4   ModelEyePos;               // the eye position in model space

float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;

int AlphaRef = 10;

float4 FogDistances         = float4( 0.0, 750.0, -25.0, 10.0);         // x=near, y=far, z=bottom, w=top

float4 FogNearBottomColor   = float4( 1.0, 1.0, 1.0, 0.0);             // rgb=color, a=intensity
float4 FogNearTopColor      = float4( 1.0, 1.0, 1.0, 0.0);             // rgb=color, a=intensity

float4 FogFarBottomColor    = float4( 0.8 , 0.85 , 0.86 , 1.0);             // rgb=color, a=intensity
float4 FogFarTopColor       = float4( 0.8 , 0.85 , 0.86 , 0.3);          // rgb=color, a=intensity

float Time;
float Height;

texture DiffMap0;
texture DiffMap1;
texture BumpMap0;
texture DiffMap2;
texture ShadowMap;
texture ShadowModMap;

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
    float2 uv1 :      TEXCOORD1;
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float2 uv0          : TEXCOORD0;        // texture coordinate
    float2 uv1          : TEXCOORD1;        // lightmap coordinate
    float3 lightVec     : TEXCOORD2;        // light vector
    float3 halfVec      : TEXCOORD3;        // half vector
//    float4 lightPos     : TEXCOORD4;        // vertex position in light projection space
    float4 shadowPos    : TEXCOORD5;        // vertex position in shadow map space
    float4 diffuse      : TEXCOORD6;        // LightDiffuse * MatDiffuse
    float4 fog          : TEXCOORD7;        // fog
    float4 specular     : COLOR0;           // LightSpecular * MatSpecular
};

struct PsOutput
{
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/lightmapsampler.fx"
#include "../lib/bumpsampler.fx"
#include "../lib/shadowmapsampler.fx"
#include "../lib/shadowmodsampler.fx"
#include "../lib/diff2sampler.fx"
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

    vsOut.shadowPos = mul(vsIn.position, ModelLightProjection);
   
    // FIXME: This is constant for the entire model! Should be done outside!
    vsOut.diffuse  = LightDiffuse * MatDiffuse;
    vsOut.specular.rgb = LightSpecular * MatSpecular;
    vsOut.specular.a   = abs(vsIn.normal.y * Height);
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv1 = vsIn.uv1;

    // position in world space for height fog
    float4 worldPos = mul(vsIn.position, Model);
    vsOut.fog = fog(vsIn.position, worldPos, ModelEyePos, FogDistances, FogNearBottomColor, FogNearTopColor, FogFarBottomColor, FogFarTopColor); 
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
    float4 shadowModulate = shadow(psIn.shadowPos, -0.001f, NoiseSampler, ShadowMapSampler, ShadowModSampler);
    
    // compute resulting color components
    float4 lightMapColor = tex2D(LightmapSampler, psIn.uv1);
    float4 diffuse  = ((diffuseColor * diffIntensity * lightMapColor * shadowModulate) + (diffuseColor * lightMapColor)) * psIn.diffuse;
    float4 specular = psIn.specular * specIntensity  * lightMapColor * shadowModulate;
    
    // compute final color
    float4 color = diffuse + specular;
    psOut.color.rgb = lerp(color.rgb, psIn.fog.rgb, psIn.fog.a);
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

                