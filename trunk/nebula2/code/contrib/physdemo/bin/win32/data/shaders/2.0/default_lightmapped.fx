#line 1 "default_lightmapped.fx"
//------------------------------------------------------------------------------
//  ps2.0/default_lightmapped.fx
//
//  A simple lightmapped shader.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 Model;                     // the model -> world matrix
shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float4x4 ModelShadowProjection;
shared float3   ModelEyePos;               // the eye position in model space
shared float3   ModelLightPos;             // the light position in model space

float Intensity0 = 2.0f;                   // hdr multiplier
texture DiffMap0;
texture DiffMap1;
texture ShadowMap;

int AlphaRef = 100;

int CullMode = 2;

static bool EnableShadows = false;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
    float2 uv1 :      TEXCOORD1;
    // float3 tangent :  TANGENT; 
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float2 uv0          : TEXCOORD0;        // texture coordinate
    float2 uv1          : TEXCOORD1;        // lightmap coordinate
//    float3 F_ex         : TEXCOORD4;        // light outscatter coefficient
//    float3 L_in         : TEXCOORD5;        // light inscatter coefficient
//    float4 shadowPos    : TEXCOORD6;        // shadow source position
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/lightmapsampler.fx"

/*
sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};
*/
// #include "../lib/bumpsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
//    vsOut.shadowPos = mul(vsIn.position, ModelShadowProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv1 = vsIn.uv1;
//    vsAthmoFog(vsIn.position, ModelEyePos, ModelLightPos, vsOut.L_in, vsOut.F_ex);    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    float4 baseColor = tex2D(DiffSampler, psIn.uv0);
    float4 lightmapColor = tex2D(LightmapSampler, psIn.uv1);
    float4 color = baseColor * lightmapColor * Intensity0;
    return color;
/*    
    if (EnableShadows)
    {
        float4 shadowModulate = shadow(psIn.shadowPos, 0.0f, ShadowMapSampler);
        color *= shadowModulate;
    }
*/    
//    return psAthmoFog(psIn.L_in, psIn.F_ex, color);
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

                