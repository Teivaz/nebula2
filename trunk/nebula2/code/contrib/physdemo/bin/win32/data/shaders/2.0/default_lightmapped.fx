//------------------------------------------------------------------------------
//  ps2.0/default_lightmapped.fx
//
//  The a simple lightmapped shader.
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

texture DiffMap0;
texture DiffMap1;
texture BumpMap0;

float4 LightDiffuse;                // light's diffuse component
float4 LightSpecular;               // light's specular component

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
    float4 diffuse      : TEXCOORD5;        // LightDiffuse * MatDiffuse
    float4 fog          : TEXCOORD6;        // fog
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

    // FIXME: This is constant for the entire model! Should be done outside!
    vsOut.diffuse  = LightDiffuse * MatDiffuse;
    vsOut.specular = LightSpecular * MatSpecular;
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
    float4 diffuseColor = tex2D(DiffSampler, psIn.uv0);

    // sample bump map
    float4 bump = tex2D(BumpSampler, psIn.uv0);
    float4 specularScale = bump.a;
    float3 surfNormal = 2.0f * (bump.xyz - 0.5f);

    // compute diffuse and specular intensity
    float diffIntensity = saturate(dot(surfNormal, normalize(psIn.lightVec)));
    float specIntensity = specularScale * pow(saturate(dot(surfNormal, normalize(psIn.halfVec))), MatSpecularPower);

    // compute resulting color components
    float4 lightMapColor = tex2D(LightmapSampler, psIn.uv1);
    float4 diffuse  = ((diffuseColor * diffIntensity * lightMapColor) + (diffuseColor * lightMapColor)) * psIn.diffuse;
    float4 specular = psIn.specular * specIntensity  * lightMapColor;

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

                