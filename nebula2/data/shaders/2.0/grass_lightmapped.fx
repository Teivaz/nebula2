//------------------------------------------------------------------------------
//  ps2.0/grass.fx
//
//  The default shader for dx9 cards using vertex/pixel shader 2.0.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/randtable.fx"

shared float4x4 ModelViewProjection;
shared float3 ModelEyePos;
shared float4 LightDiffuse;

float4 MatDiffuse;
float4 MatAmbient;
float Intensity0 = 2.0f;            // hdr multiplier

float4 DisplayResolution = { 640.0, 480.0, 0.0, 0.0};
float MinSpriteSize = 0.1f;
float MaxSpriteSize = 1.0f;
float4 Noise = float4(0.5f, 0.25f, 0.5f, 0.0f);   // randomize scaler
float3 RandPosScale = { 1234.5f, 3021.7f, 2032.1f };
float MinDist = 90.0f;
float MaxDist = 110.0f;

texture DiffMap0;   // base texture map
texture DiffMap1;   // lightmap
texture DiffMap3;   // sprite texture

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float2 uv1 :      TEXCOORD1;    
    float3 tangent  : TANGENT;
};

struct VsOutput
{
    float4 position : POSITION;
    float4 color : COLOR1;
    float  psize : PSIZE;
    float4 uvx : COLOR0;
};

struct PsInput
{
    float2 uv0 : TEXCOORD0;
    float4 uvx : COLOR0;
    float4 color : COLOR1;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"
#include "../lib/lightmapsampler.fx"

sampler SpriteSampler = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // get a random number
    float4 rnd = RandArray[fmod(dot(RandPosScale, abs(vsIn.position.xyz)), 16.0f)];
    
    // compute a randomized size
    float size = lerp(MinSpriteSize, MaxSpriteSize, abs(rnd.x));
    
    // compute the binormal 
    float3 binormal = cross(vsIn.normal, vsIn.tangent);
    
    // compute random displacement
    float3 displace = rnd * (vsIn.tangent * Noise.x + vsIn.normal * Noise.y + binormal * Noise.z);
    
    // add above-ground-offset
    displace += vsIn.normal * size * 0.3f;
    float4 pos = vsIn.position + float4(displace, 0.0f);
    vsOut.position = mul(pos, ModelViewProjection);
    
    // compute point size
    float dist = distance(ModelEyePos, vsIn.position);
    vsOut.psize = DisplayResolution.y * size * (1.0f / dist);
    
    // compute lighting...
    vsOut.color = MatDiffuse;
    vsOut.color.a = saturate((MinDist - dist) / (MaxDist - MinDist)); 
    vsOut.uvx.xy = frac(vsIn.uv0);   
    vsOut.uvx.zw = vsIn.uv1;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const PsInput psIn) : COLOR
{
    float4 spriteColor = tex2D(SpriteSampler, psIn.uv0);
    float4 lightmapColor = tex2D(LightmapSampler, psIn.uvx.zw);    
    float4 baseColor = tex2D(DiffSampler, psIn.uvx.xy);
    return spriteColor * baseColor * lightmapColor * psIn.color * LightDiffuse * Intensity0;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode = None;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;                
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
