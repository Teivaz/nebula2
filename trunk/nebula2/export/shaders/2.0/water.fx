#line 1 "water.fx"
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

float4 WaterColor = float4(0.299f, 0.392f, 0.452f, 1.0f); 
float WaterColorIntensity = 0.6;

texture BumpMap0;                   // wave bump
texture CubeMap0;                   // cubic environment map
texture LightModMap;                // the light modulation map

float Time;                         // the current time

float Scale = 10.0f;
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
    { -0.0368069, 0.00253061, 1.52005, 0.241924 },
    { 0.00235418, 0.00267063, 2.7365, 0.435527},
    { -0.0298042, -0.0892409, 2.52404, 0.401714},
    {0.0361108 , 0.0233059, 2.03394,0.323711 },
    { -0.0109379, -0.00972796,3.46456 ,0.551403  },
};

float2 WaveDirs[] = {
   0.470025 , 0.170519,
   0.387961 , -0.315415,
   0.499167 , -0.028842,
   0.464146 , -0.185926,
   0.373996 , 0.331854,
};

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
    float2 uv0      : TEXCOORD0;        // bump 0 tex coord
    float2 uv1      : TEXCOORD1;        // bump 1 tex coord
    float2 uv2      : TEXCOORD2;        // bump 2 tex coord
    float2 uv3      : TEXCOORD3;        // bump 3 tex coord
    float3 view     : TEXCOORD4;        // view vector
    float4 fog      : TEXCOORD7;        // fog
};

struct PsOutput
{
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler Bump0Sampler = sampler_state
{
    Texture   = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
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
        height.y += WaveParams[i].x * Scale * 
             (float) sin(WaveParams[i].y * WaveParams[i].z * WaveParams[i].w * Frequency * 
             (Time + WaveDirs[i].x * vsIn.position.x + WaveDirs[i].y * vsIn.position.z));             
    }
    float4 pos = vsIn.position + height;
    
    // transform vertex position
    vsOut.position = mul(pos, ModelViewProjection);

    // world space view vector
    float3 worldPos = mul(vsIn.position, Model);
    float3 worldEye = mul(ModelEyePos, Model);
    vsOut.view = worldEye - worldPos;
                                
    // generate bump texture coordinates
    vsOut.uv0 = 0.00005 * (vsIn.position.xz + Time * WaveDirs[0] * Wave0Speed);
    vsOut.uv1 = 0.0005 * (vsIn.position.xz + Time * WaveDirs[1] * Wave1Speed);
    vsOut.uv2 = 0.005 * (vsIn.position.xz + Time * WaveDirs[2] * Wave2Speed);
    vsOut.uv3 = 0.005 * (vsIn.position.xz + Time * WaveDirs[3] * Wave3Speed);
    
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
    float4 bump0 = 2 * tex2D(Bump0Sampler, psIn.uv0) - 1;
    float4 bump1 = 2 * tex2D(Bump0Sampler, psIn.uv1) - 1;
    float4 bump2 = 2 * tex2D(Bump0Sampler, psIn.uv2) - 1;
    float4 bump3 = 2 * tex2D(Bump0Sampler, psIn.uv3) - 1;
    
    // add bump1 offset on bump0
//    bump0 = (bump0 + bump1 + bump2 + bump3) * 0.25;
    // FIXME: Fallback to "old" incorrect water
    bump0.xz = (bump0 + bump1 + bump2 + bump3) * 0.25;
  
    // compute reflected/refracted view vector
    /*
    float3 view = normalize(psIn.view);
    float frsnl = 1.0f - saturate(dot(view, bump0.xzy));
    float3 reflectedView = reflect(-view, bump0.xzy);
    */
    // FIXME: Fallback to "old" incorrect water
    float3 reflectedView = reflect(psIn.view, bump0);
    
    // get reflected/refracted color
    float4 reflectedColor = texCUBE(EnvSampler, reflectedView);
    
    // do cubemap lookup
/*    
    float4 color = lerp(MatDiffuse, reflectedColor * MatSpecular, frsnl) * lightModulate;
*/
    float4 color = lerp(WaterColor, reflectedColor, WaterColorIntensity);
    psOut.color = lerp(color, psIn.fog, psIn.fog.a);
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
        
        AlphaTestEnable  = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
