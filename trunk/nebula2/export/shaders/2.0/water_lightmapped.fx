//------------------------------------------------------------------------------
//  2.0/water.fx
//
//  Implements a water surface shader.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 Model;                     // the model matrix
shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float4x4 ModelLightProjection;      // the model*light*projection matrix
shared float3 ModelEyePos;                 // the eye position in model space

float4 MatDiffuse;                  // the water color
float MatTransparency;              // the water color "intensity"

int AlphaRef = 10;

texture DiffMap1;
texture BumpMap0;                   // wave bump
texture CubeMap0;                   // cubic environment map
texture LightModMap;                // the light modulation map

float Time;                         // the current time

float Height = 10.0f;
float Frequency = 10.0f;

float Wave0Speed = 2.5f;
float Wave1Speed = -2.5f;
float Wave2Speed = 7.0f;
float Wave3Speed = -7.5f;

float4 FogDistances         = float4( 0.0, 750.0, -25.0, 10.0);         // x=near, y=far, z=bottom, w=top

float4 FogNearBottomColor   = float4( 1.0, 1.0, 1.0, 0.01);             // rgb=color, a=intensity
float4 FogNearTopColor      = float4( 1.0, 1.0, 1.0, 0.00);             // rgb=color, a=intensity

float4 FogFarBottomColor    = float4( 0.8 , 0.85 , 0.86 , 1.0);             // rgb=color, a=intensity
float4 FogFarTopColor       = float4( 0.8 , 0.85 , 0.86 , 0.3);          // rgb=color, a=intensity

float2 Wave0Dir = float2(1.0f, 0.0f);
float2 Wave1Dir = float2(1.0f, 0.0f);
float2 Wave2Dir = float2(0.0f, 1.0f);
float2 Wave3Dir = float2(0.707f, 0.707f);

// wave constants for 5 waves: amplitude, phase, omega, freq
float4 WaveParams[] = {
    { 0.147805, -0.0121289, 1.31949, 0.210004 },
    { -0.0746641, -0.00838156, 1.43167, 0.227857 },
    { 0.0399901, -0.140617, 2.5833, 0.411145 },
    { -0.0591525, 0.138653, 2.31409, 0.368299 },
    { -0.146556, 0.0107259, 1.58717, 0.252607 },
};

float2 WaveDirs[] = {
    0.474854, -0.156568,
    0.309276, 0.392872,
    0.492071, -0.0886908,
    0.485016, 0.12149,
    0.499876, -0.0111204,
};

int CullMode = 2;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION;
    float3 normal :   NORMAL;  
    float3 tangent :  TANGENT; 
    float2 uv0 :      TEXCOORD0;
    float2 uv1 :      TEXCOORD1;    
};

struct VsOutput
{
    float4 position   : POSITION;       // position in projection space
    float2 uv0        : TEXCOORD0;      // bump 0 tex coord
    float2 uv1        : TEXCOORD1;      // bump 1 tex coord
    float2 uv2        : TEXCOORD2;      // bump 2 tex coord
    float2 uv3        : TEXCOORD3;      // bump 3 tex coord
    float3 view       : TEXCOORD4;      // view vector
    float4 lightPos   : TEXCOORD5;      // vertex position in light projection space    
    float2 lightmapUv : TEXCOORD6;      // lightmap uv coords  
    float4 fog        : COLOR0;         // fog
};

struct PsOutput
{
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "shaders:../lib/lightsampler.fx"
#include "shaders:../lib/lightmapsampler.fx"

sampler Bump0Sampler = sampler_state
{
    Texture   = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler EnvSampler = sampler_state
{
    Texture = <CubeMap0>;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;    
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;

    // compute wave height
    const int Amplitude = 0;
    const int Phase = 1;
    const int Omega = 2;
    const int Freq  = 3;
    const int DirX  = 4;
    const int DirY  = 5;
    int i = 0;
    float4 height = { 0.0f, 0.0f, 0.0f, 0.0f };
    for (i = 0; i < 5; i++)
    {
        height.y += WaveParams[i].x * Height * 
             (float) sin(WaveParams[i].y * WaveParams[i].z * WaveParams[i].w * Frequency * 
             (Time + WaveDirs[i].x * vsIn.position.x + WaveDirs[i].y * vsIn.position.z));             
    }
    float3 pos = vsIn.position + height;
    
    // transform vertex position
    vsOut.position = transformStatic(pos, ModelViewProjection);

    // for drifting clouds...
    vsOut.lightPos = transformStatic(pos, ModelLightProjection);
    vsOut.lightPos.x += Time * 0.15;
    vsOut.lightPos.y += Time * 0.15;
    vsOut.lightPos.w *= 7.5f;
    
    // world space view vector
    float3 worldEye = transformStatic(ModelEyePos, Model);
    float3 worldPos = transformStatic(vsIn.position, Model);
    vsOut.view = normalize(worldEye - worldPos);
                                
    // generate bump texture coordinates
    vsOut.uv0 = 0.00005 * (vsIn.position.xz + Time * WaveDirs[0] * Wave0Speed);
    vsOut.uv1 = 0.0005 * (vsIn.position.xz + Time * WaveDirs[1] * Wave1Speed);
    vsOut.uv2 = 0.005 * (vsIn.position.xz + Time * WaveDirs[2] * Wave2Speed);
    vsOut.uv3 = 0.005 * (vsIn.position.xz + Time * WaveDirs[3] * Wave3Speed);
    vsOut.lightmapUv = vsIn.uv1;
    
    vsOut.fog = fog(vsIn.position, worldPos, ModelEyePos, FogDistances, FogNearBottomColor, FogNearTopColor, FogFarBottomColor, FogFarTopColor); 

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
PsOutput psMain(const VsOutput psIn)
{
    PsOutput psOut;
    
    // sample wave0 bump
    float4 bump0 = 2.0f * (tex2D(Bump0Sampler, psIn.uv0) - 0.5f);
    float4 bump1 = 2.0f * (tex2D(Bump0Sampler, psIn.uv1) - 0.5f);
    float4 bump2 = 2.0f * (tex2D(Bump0Sampler, psIn.uv2) - 0.5f);
    float4 bump3 = 2.0f * (tex2D(Bump0Sampler, psIn.uv3) - 0.5f);
    
    // add bump1 offset on bump0
    bump0.xz = (bump0 + bump1 + bump2 + bump3) * 0.25;
//    bump0.xz = (bump0 + bump1) * 0.5f;
  
    // compute reflected view vector
    float3 reflectedView = reflect(psIn.view, bump0);
    
    // get reflected color
    float3 reflectedColor = texCUBE(EnvSampler, reflectedView);
    
    // get lightmap color
    float4 lightMapColor = tex2D(LightmapSampler, psIn.lightmapUv);
    
    // sample light modulation color
    float4 lightModulate;
    if (psIn.lightPos.w >= 0.0f)
    {
        lightModulate = tex2Dproj(LightSampler, psIn.lightPos);
    }
    else
    {
        lightModulate = float4(0.0, 0.0, 0.0, 0.0);
    }
    
    // do cubemap lookup
    float3 color = lightMapColor * lerp(MatDiffuse, reflectedColor, MatTransparency) * lightModulate;

    psOut.color.rgb = lerp(color.rgb, psIn.fog.rgb, psIn.fog.a);
    psOut.color.a   = 1.0f;
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
