#line 1 "terrain.fx"
//------------------------------------------------------------------------------
//  ps2.0/terrain.fx
//
//  The default shader for dx9 cards using vertex/pixel shader 2.0,
//  running in 1 render pass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelShadowProjection;
shared float4x4 ModelViewProjection;        // the model*view*projection matrix
shared float3 ModelEyePos;
shared float3 ModelLightPos;

float4 MatDiffuse;                  // material diffuse color

float3 SunPos = float3(-100000, 2000, -26800);

float4 TexGenS;                     // texgen parameters for u
float4 TexGenT;                     // texgen parameters for v

texture AmbientMap0;                // material weight texture
texture DiffMap0;                   // grass tile texture
texture DiffMap1;                   // rock tile texture
texture DiffMap2;                   // ground tile texture
texture DiffMap3;                   // snow tile texture
texture ShadowMap;

int AlphaRef = 100;
int CullMode = 2;

float4 FogColor = { 142.0 / 255.0, 140.0 / 255.0, 163.0 / 255.0, 1.0 };
float DetailEnd = 150.0f;
float TexScale = 0.008f;
float DetailTexScale = 0.1f;

float4 SnowAmplify = { 1.2, 1.2, 1.3, 0.0 };

static bool EnableShadows = true;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv0 : TEXCOORD0;             // generated material weight texture coordinates
    float2 uv1 : TEXCOORD1;             // generated tile texture coordinates
    float2 uv2 : TEXCOORD2;             // detail texture coordinates
    float1 fog : TEXCOORD3;             // x: reldist
    float3 F_ex : TEXCOORD4;            // light outscatter coefficient
    float3 L_in : TEXCOORD5;            // light inscatter coefficient
    float4 shadowPos : TEXCOORD6;        // the light position
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler WeightSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

sampler GrassSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler RockSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler GroundSampler = sampler_state
{
    Texture = <DiffMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler SnowSampler = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.shadowPos = mul(vsIn.position, ModelShadowProjection);
    
    // generate texture coordinates after OpenGL rules
    vsOut.uv0.x = dot(vsIn.position, TexGenS);
    vsOut.uv0.y = dot(vsIn.position, TexGenT);
    vsOut.uv1.xy = vsIn.position.xz * TexScale;
    vsOut.uv2.xy = vsIn.position.xz * DetailTexScale;        
    float eyeDist = distance(ModelEyePos, vsIn.position);
    vsOut.fog.x = 1.0f - saturate(eyeDist / DetailEnd);

    vsAthmoFog(vsIn.position, ModelEyePos, SunPos, vsOut.L_in, vsOut.F_ex);
    vsOut.F_ex *= saturate(lerp(SunColor, 1.0, 0.3));

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // sample material weight texture
    float4 matWeights = tex2D(WeightSampler, psIn.uv0);
    
    // sample tile textures
    float4 baseColor = matWeights.x * lerp(tex2D(GrassSampler, psIn.uv1), tex2D(GrassSampler, psIn.uv2), psIn.fog.x);
    baseColor += matWeights.y * lerp(tex2D(RockSampler, psIn.uv1), tex2D(RockSampler, psIn.uv2), psIn.fog.x);
    baseColor += matWeights.z * lerp(tex2D(GroundSampler, psIn.uv1), tex2D(GroundSampler, psIn.uv2), psIn.fog.x);
    baseColor += matWeights.w * SnowAmplify * tex2D(SnowSampler, psIn.uv1);
    
    if (EnableShadows)
    {
        float4 shadowModulate = shadow(psIn.shadowPos, 0.0f, ShadowMapSampler);
        baseColor *= shadowModulate;
    }
    
    return psAthmoFog(psIn.L_in, psIn.F_ex, baseColor);
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
        // FillMode = Wireframe;
        
        AlphaTestEnable  = False;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
