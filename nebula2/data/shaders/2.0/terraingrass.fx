#line 1 "terraingrass.fx"
//------------------------------------------------------------------------------
//  ps2.0/terraingrass.fx
//
//  A point sprite based grass shader.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/randtable.fx"

shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float4x4 ModelLightProjection;      // the model*light*projection matrix
shared float3   ModelEyePos;               // the eye position in model space
shared float Time;
shared float4 DisplayResolution = { 640.0, 480.0, 0.0, 0.0};

float4 LightDiffuse;

float4 TexGenS;                     // texgen parameters for u
float4 TexGenT;                     // texgen parameters for v
float TexScale = 0.1;

texture AmbientMap0;                // material weight texture
texture SpecMap0;                   // the actuall grass brush
texture DiffMap0;                   // grass tile texture
texture DiffMap1;                   // rock tile texture
texture DiffMap2;                   // ground tile texture
texture DiffMap3;                   // snow tile texture

float2 FadeDist = { 90.0, 110.0f };  // grass fade distances

float MinSpriteSize = 0.7f;
float MaxSpriteSize = 1.0f;
float SpriteSwingTime = 1.0f;
float SpriteSwingTranslate = 0.1f;
float4 Noise = float4(0.5f, 0.25f, 0.5f, 0.0f);   // randomize scaler
float3 RandPosScale = { 1234.5f, 321.7f, 5432.1f };

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
    float2 uv0 : TEXCOORD0;
    float4 uvx : COLOR0;                // generated material weight texture coordinates
    float4 color : COLOR1;              // alpha is fade value!
    float  psize : PSIZE;
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
sampler WeightSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler BrushSampler = sampler_state
{
    Texture = <SpecMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    // MipMapLodBias = -0.5f;
};

sampler GrassSampler  = sampler_state
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
    
    // get a random number
    float4 rnd = RandArray[fmod(dot(RandPosScale, vsIn.position.xyz), 16.0f)];
    
    // compute a randomized size
    float dist = distance(ModelEyePos, vsIn.position);
    float size = lerp(MinSpriteSize, MaxSpriteSize, abs(rnd.x));
    float4 pos = vsIn.position + float4(0.0f, size * 0.5f, 0.0f, 0.0f);
    /*
    if (dist < 20.0f)
    {
        pos.x += sin(Time * SpriteSwingTime + vsIn.position.x * 0.5f) * SpriteSwingTranslate;    
    }
    */
    vsOut.position = mul(pos, ModelViewProjection);    
    
    // compute point size
    vsOut.psize = DisplayResolution.y * size * (1.0f / dist);
    
    // compute texture coords 
    vsOut.uv0 = float2(0.0, 0.0);
    vsOut.uvx.x = dot(vsIn.position, TexGenS);
    vsOut.uvx.y = dot(vsIn.position, TexGenT);
    vsOut.uvx.zw = fmod(vsIn.position.xz * TexScale, 1.0f);
    
    // compute fade value
    vsOut.color.rgb = lerp(LightDiffuse.rgb, 1.0, 0.3);
    vsOut.color.a = saturate((FadeDist.x - dist) / (FadeDist.y - FadeDist.x));
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const PsInput psIn) : COLOR
{
    // sample the grass brush color
    float4 brushColor = tex2D(BrushSampler, psIn.uv0);

    // sample material weight texture
    float4 matWeights = tex2D(WeightSampler, psIn.uvx.xy);

    // sample tile textures
    float4 baseColor = matWeights.x * tex2D(GrassSampler, psIn.uvx.zw);
    baseColor += matWeights.y * tex2D(RockSampler, psIn.uvx.zw);
    baseColor += matWeights.z * tex2D(GroundSampler, psIn.uvx.zw);
    baseColor += matWeights.w * tex2D(SnowSampler, psIn.uvx.zw);
    
    float4 color = baseColor * brushColor * psIn.color;

    return color;
}

//------------------------------------------------------------------------------
//  The technique
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode  = None;
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;        
        PointSpriteEnable = True;
        PointScaleEnable  = False;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
