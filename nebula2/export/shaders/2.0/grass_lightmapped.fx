//------------------------------------------------------------------------------
//  ps2.0/grass_lightmapped.fx
//
//  A point sprite based grass shader with lightmapped grass.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/randtable.fx"

float4x4 Model;                     // the model -> world matrix
float4x4 ModelViewProjection;       // the model*view*projection matrix
float4x4 ModelLightProjection;      // the model*light*projection matrix
float4   ModelLightPos;             // the light's position in model space
float4   ModelEyePos;               // the eye position in model space

int AlphaRef = 10;

float Time;

float4 DisplayResolution;
float MinSpriteSize = 0.1f;
float MaxSpriteSize = 0.1f;
float SpriteSwingTime = 1.0f;
float SpriteSwingTranslate = 0.5f;
float4 Noise = float4(0.5f, 0.25f, 0.5f, 0.0f);   // randomize scaler
float3 RandPosScale = { 1234.5f, 321.7f, 5432.1f };

float4 MatDiffuse;

texture DiffMap1;   // lightmap texture
texture DiffMap3;   // grass texture

int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv0      : TEXCOORD0;        // contains the lightmap coordinates
};

struct VsOutput
{
    float4 position : POSITION;         // position in projection space
    float2 uv1 : COLOR;                 // lightmap texture coordinates
    float  psize :    PSIZE;
};

struct PsInput
{
    float2 uv0 : TEXCOORD0;             // point sprite texture coordinates
    float4 uv1 : COLOR;             // lightmap texture coordinates
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = None;
};
#include "../lib/lightmapsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // get a random number
    float4 rnd = RandArray[fmod(dot(RandPosScale, vsIn.position.xyz), 16.0f)];
    
    // compute a randomized size
    float size = lerp(MinSpriteSize, MaxSpriteSize, abs(rnd.x));
    
    // compute the binormal 
    float3 binormal = cross(vsIn.normal, vsIn.tangent);
    
    // compute random displacement
    float3 displace = rnd * (vsIn.tangent * Noise.x + vsIn.normal * Noise.y + binormal * Noise.z);
    
    // add above-ground-offset
    displace += vsIn.normal * size * 0.3f;
    float3 pos = vsIn.position + displace;

    pos += vsIn.tangent * sin(Time * SpriteSwingTime + vsIn.position.z) * SpriteSwingTranslate;
    vsOut.position = mul(float4(pos, 1.0f), ModelViewProjection);
    
    // compute point size
    float dist  = distance(ModelEyePos, vsIn.position);    
    vsOut.psize = DisplayResolution.y * size * (1.0f / dist);
    vsOut.uv1   = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const PsInput psIn) : COLOR
{
    float4 texColor = tex2D(DiffSampler, psIn.uv0) * MatDiffuse;
    float4 lightMapColor = tex2D(LightmapSampler, psIn.uv1);
    return texColor * lightMapColor;
}

//------------------------------------------------------------------------------
//  The technique
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
        
        PointSpriteEnable = True;
        PointScaleEnable  = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
