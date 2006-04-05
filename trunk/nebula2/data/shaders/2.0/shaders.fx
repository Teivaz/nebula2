//------------------------------------------------------------------------------
//  shaders.fx
//
//  Nebula2 standard shaders for shader model 2.0
//
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    shader parameters

    NOTE: VS_PROFILE and PS_PROFILE macros are usually provided by the
    application and contain the highest supported shader models.
*/
/*
#define VS_PROFILE vs_2_0
#define PS_PROFILE ps_2_0
*/

#define DIRLIGHTS_ENABLEOPPOSITECOLOR 1
#define DIRLIGHTS_OPPOSITECOLOR float3(0.45f, 0.52f, 0.608f)
#define DEBUG_LIGHTCOMPLEXITY 0

#define DECLARE_SCREENPOS(X) float4 screenPos : X;
#define PS_SCREENPOS psComputeScreenCoord(psIn.screenPos)
#define VS_SETSCREENPOS(X) vsOut.screenPos = vsComputeScreenCoord(X);

typedef half3 color3;
typedef half4 color4;

shared float4   HalfPixelSize;
shared float4x4 InvModelView;
shared float4x4 ModelViewProjection;
shared float4x4 Model;
shared float3   ModelEyePos;
shared float4x4 Light;

shared float4x4 TextureTransform0;
shared float4x4 TextureTransform1;

float4x4 MLPUVStretch = {40.0f,  0.0f,  0.0f,  0.0f,
                         0.0f, 40.0f,  0.0f,  0.0f,
                         0.0f,  0.0f, 40.0f,  0.0f,
                         0.0f,  0.0f,  0.0f, 40.0f };

// detail texture scale for the layered shader, zoom by 40 as default
static const float4x4 DetailTexture = {40.0f,  0.0f,  0.0f,  0.0f,
                                        0.0f, 40.0f,  0.0f,  0.0f,
                                        0.0f,  0.0f, 40.0f,  0.0f,
                                        0.0f,  0.0f,  0.0f, 40.0f };

//static float w = 4.0f;      // width  for orthogonal projection
//static float h = 3.0f;      // height for orthogonal projection
//static float zn = -1.0f;    // z near for orthogonal projection
//static float zf = 1.0f;     // z far  for orthogonal projection
// orthogonal RH projection matrix, used by the 3d gui shader
/*
static const float4x4 OrthoProjection = {2/w,   0,      0,          0,
                                         0,     2/h,    0,          0,
                                         0,     0,      1/(zn-zf),  0,
                                         0,     0,      zn/(zn-zf), 1};
*/
static const float4x4 OrthoProjection = {0.5,   0,      0,      0,
                                         0,     0.6667, 0,      0,
                                         0,     0,      -0.5,   0,
                                         0,     0,       0.5,   1};

shared float Time;
shared float4 DisplayResolution;

float3 ModelLightPos;

shared float3   LightPos;
shared int      LightType;
shared float    LightRange;
shared float4   LightDiffuse;
shared float4   LightSpecular;
shared float4   LightAmbient;
shared float4   ShadowIndex;

float BumpScale = 0.0f;
float CubeLightScale;

float4 Color0;
float4 MatDiffuse;
float4 MatEmissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
float  MatEmissiveIntensity = 0.0f;
float4 MatSpecular;
float4 MatAmbient;
float  MatSpecularPower;

bool  AlphaBlendEnable = true;
float Intensity0;

matrix<float,4,3> JointPalette[72];
int AlphaRef;
int CullMode = 2;
int AlphaSrcBlend = 5;
int AlphaDstBlend = 6;
int VertexStreams;
float4 VertexWeights1;
float4 VertexWeights2;
float4 TexGenS;                     // texgen parameters for u
float4 TexGenT;                     // texgen parameters for v

float4 Wind;                        // wind direction, strength in .w
float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max
float3 BoxCenter;                   // model space bounding box center
float SpriteSwingTime;              // swing period
float SpriteSwingAngle;             // swing angle modulator
float InnerLightIntensity;          // light intensity at center
float OuterLightIntensity;          // light intensity at periphery
float MinSpriteSize = 0.1f;
float MaxSpriteSize = 1.0f;
float4 Noise;
float3 RandPosScale = { 1234.5f, 3021.7f, 2032.1f };
float MinDist = 90.0f;
float MaxDist = 110.0f;

static const float DetailEnd = 150.0f;
static const float TexScale = 0.008f;
static const float DetailTexScale = 0.1f;
static const float4 SnowAmplify = { 1.2, 1.2, 1.3, 0.0 };
static const float4 GenAngle = { 3.54824948311, -11.6819286346, 10.4263944626, -2.29271507263 };
static const float MaxAtmoDist = 1000.0;
static const float MaxWaterDist = 4.0;
float4 Velocity = float4(0.0, 0.0, 0.0, 0.0); // UVAnimaton2D, Water movement

texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
texture DiffMap3;
texture BumpMap0;
texture BumpMap1;
texture CubeMap0;
texture NoiseMap0;
texture NoiseMap1;
texture NoiseMap2;
texture NoiseMap3;
texture AmbientMap0;
texture LightModMap;
shared texture AmbientMap1;

#include "shaders:../lib/lib.fx"
#include "shaders:../lib/randtable.fx"

//#include "c:/radonlabs/data/shaders/lib/lib.fx"
//#include "c:/radonlabs/data/shaders/lib/randtable.fx"

//------------------------------------------------------------------------------
/**
    shader input/output structure definitions
*/

struct vsInputStaticDepth
{
    float4 position : POSITION;
};

struct vsOutputStaticDepth
{
    float4 position : POSITION;
    float  depth : TEXCOORD0;
};

struct vsInputUvDepth
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct vsOutputUvDepth
{
    float4 position : POSITION;
    float3 uv0depth : TEXCOORD0;
};

struct vsInputStaticColor
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
};

struct vsOutputStaticColor
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 lightVec      : TEXCOORD1;
    float3 modelLightVec : TEXCOORD2;
    float3 halfVec       : TEXCOORD3;
    float3 eyePos        : TEXCOORD4;
    DECLARE_SCREENPOS(TEXCOORD5)
};

struct vsOutputSoftSilhouette
{
    float4 position      : POSITION;
    float3 uv0silhouette : TEXCOORD0;
    float3 lightVec      : TEXCOORD1;
    float3 modelLightVec : TEXCOORD2;
    float3 halfVec       : TEXCOORD3;
    float3 eyePos        : TEXCOORD4;
    DECLARE_SCREENPOS(TEXCOORD5)
};

struct vsOutputRPLStaticColor
{
    float4 position         : POSITION;
    float2 uv0              : TEXCOORD0;
    float3 lightVec         : TEXCOORD1;
    float3 modelLightVec    : TEXCOORD2;
    float3 halfVec          : TEXCOORD3;
    float3 eyePos           : TEXCOORD4;
    DECLARE_SCREENPOS(TEXCOORD5)
};

struct vsInputStatic2Color
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
    float3 tangent  : TANGENT;
    float3 binormal  : BINORMAL;
};

struct vsOutputStatic2Color
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float2 uv1           : TEXCOORD1;
    float3 lightVec      : TEXCOORD2;
    float3 modelLightVec : TEXCOORD3;
    float3 halfVec       : TEXCOORD4;
    float3 eyePos        : TEXCOORD5;
    DECLARE_SCREENPOS(TEXCOORD6)
};

struct vsInputStatic3Color
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 binormal  : BINORMAL;
};

struct vsOutputStatic3Color
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float2 uv1           : TEXCOORD1;
    float2 uv2           : TEXCOORD2;
    float3 lightVec      : TEXCOORD3;
    float3 modelLightVec : TEXCOORD4;
    float3 halfVec       : TEXCOORD5;
    float3 eyePos        : TEXCOORD6;
    DECLARE_SCREENPOS(TEXCOORD7)
};


struct vsOutputEnvironmentColor
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 lightVec      : TEXCOORD1;
    float3 modelLightVec : TEXCOORD2;
    float3 halfVec       : TEXCOORD3;
    float3 worldReflect  : TEXCOORD4;
    float3 eyePos        : TEXCOORD5;
    DECLARE_SCREENPOS(TEXCOORD6)
};

struct vsInputLightmappedColor
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0 :      TEXCOORD0;
    float2 uv1 :      TEXCOORD1;
    float3 tangent  : TANGENT;
    float3 binormal  : BINORMAL;
};

struct vsOutputLightmappedColor
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float2 uv1          : TEXCOORD1;
};

struct vsOutputRNLightmappedColor
{
    float4 position         : POSITION;
    float2 uv0              : TEXCOORD0;
    float2 uv1              : TEXCOORD1;
    float3 eyePos           : TEXCOORD2;
    DECLARE_SCREENPOS(TEXCOORD3)
};

struct vsInputSkinnedDepth
{
    float4 position : POSITION;
    //float2 uv0 : TEXCOORD0;
    float4 weights : BLENDWEIGHT;
    float4 indices : BLENDINDICES;
};

struct vsInputSkinnedColor
{
    float4 position : POSITION;
    float3 normal :   NORMAL;
    float3 tangent :  TANGENT;
    float3 binormal  : BINORMAL;
    float2 uv0 :      TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct vsInputBlendedDepth
{
    float4 position0 : POSITION0;
    float4 position1 : POSITION1;
    float4 position2 : POSITION2;
    float4 position3 : POSITION3;
    float4 position4 : POSITION4;
    float4 position5 : POSITION5;
    float4 position6 : POSITION6;
    float4 position7 : POSITION7;
    //float2 uv0       : TEXCOORD0;
};

struct vsInputBlendedColor
{
    float4 position0 : POSITION0;
    float4 position1 : POSITION1;
    float4 position2 : POSITION2;
    float4 position3 : POSITION3;
    float4 position4 : POSITION4;
    float4 position5 : POSITION5;
    float4 position6 : POSITION6;
    float4 position7 : POSITION7;
    float3 normal0   : NORMAL0;
    float3 normal1   : NORMAL1;
    float3 normal2   : NORMAL2;
    float3 normal3   : NORMAL3;
    float3 normal4   : NORMAL4;
    float3 normal5   : NORMAL5;
    float3 normal6   : NORMAL6;
    float3 normal7   : NORMAL7;
    float2 uv0       : TEXCOORD0;
    float3 tangent   : TANGENT;
};

struct vsInputTerrainColor
{
    float4 position : POSITION;
};

struct vsOutputTerrainColor
{
    float4 position : POSITION;         // position in projection space
    float2 uv0 : TEXCOORD0;             // generated material weight texture coordinates
    float2 uv1 : TEXCOORD1;             // generated tile texture coordinates
    float2 uv2 : TEXCOORD2;             // detail texture coordinates
    float1 fog : TEXCOORD3;             // x: reldist
};

struct vsInputLeafDepth
{
    float4 position : POSITION;
    float3 extrude  : NORMAL;           // sprite extrusion vector
    float2 uv0 : TEXCOORD0;
};

struct vsInputLeafColor
{
    float4 position : POSITION;
    float3 extrude  : NORMAL;           // sprite extrusion vector
    float2 uv0 : TEXCOORD0;
};

struct vsOutputLeafColor
{
    float4 position      : POSITION;
    float3 uv0intensity  : TEXCOORD0;
    float3 lightVec      : TEXCOORD1;
    float3 modelLightVec : TEXCOORD2;
    float3 halfVec       : TEXCOORD3;
    float3 eyePos        : TEXCOORD4;
    DECLARE_SCREENPOS(TEXCOORD5)
};

struct vsOutputSpriteColor
{
    float4 position : POSITION;
    float4 color : COLOR;
    float  psize : PSIZE;
    float2 uvx : COLOR1;
};

struct psInputSpriteColor
{
    float2 uv0 : TEXCOORD0;
    float2 uvx : COLOR1;
    float4 color : COLOR0;
};

struct vsOutputSpriteLightmappedColor
{
    float4 position : POSITION;
    float4 color : COLOR;
    float  psize : PSIZE;
    float4 uvx : COLOR1;
};

struct psInputSpriteLightmappedColor
{
    float2 uv0 : TEXCOORD0;
    float4 uvx : COLOR1;
    float4 color : COLOR0;
};

struct vsInputCubeSkyboxColor
{
    float4 position : POSITION;
};

struct vsOutputCubeSkyboxColor
{
    float4 position : POSITION;
    float3 uv0 : TEXCOORD0;
};

struct vsInputSkyboxColor
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct vsOutputSkyboxColor
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct vsInputParticleColor
{
    float4 position  : POSITION;  // the particle position in world space
    float2 uv0       : TEXCOORD0; // the particle texture coordinates
    float2 extrude   : TEXCOORD1; // the particle corner offset
    float2 transform : TEXCOORD2; // the particle rotation and scale
    float4 color     : COLOR0;    // the particle color
};

struct vsOutputParticleColor
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

struct vsInputParticle2Color
{
    float4 position  : POSITION;    // the particle position in world space
    float3 velocity  : NORMAL;      // the particle coded uv and corners,rotation and scale
    float2 uv        : TEXCOORD0;   // the particle coded uv and corners,rotation and scale
    float4 data      : COLOR0;      // the particle coded uv and corners,rotation and scale
};

struct vsOutputParticle2Color
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

struct vsInputCompose
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct vsOutputCompose
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

//------------------------------------------------------------------------------
/**
    sampler definitions
*/
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler Diff1Sampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler Diff2Sampler = sampler_state
{
    Texture = <DiffMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler Diff3Sampler = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler MultiLayeredLightmapSampler = sampler_state
{
    Texture = <NoiseMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler BumpSampler = sampler_state
{
    Texture   = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

sampler EnvironmentSampler = sampler_state
{
    Texture = <CubeMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler LightmapSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler RadiosityNMSampler1 = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler RadiosityNMSampler2 = sampler_state
{
    Texture = <DiffMap2>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler RadiosityNMSampler3 = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler RadiosityNMSampler4 = sampler_state
{
    Texture = <BumpMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler WeightSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler ShadowSampler = sampler_state
{
    Texture = <AmbientMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
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

sampler SpriteSampler = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
};

sampler ComposeSourceBuffer = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

sampler ComposeSourceBuffer1 = sampler_state
{
    Texture = <DiffMap1>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

sampler Layer0Sampler = sampler_state
{
    Texture = <NoiseMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler Layer1Sampler = sampler_state
{
    Texture = <NoiseMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler Layer2Sampler = sampler_state
{
    Texture = <NoiseMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler Layer3Sampler = sampler_state
{
    Texture = <NoiseMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler SkySampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler Transparency1Sampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler Transparency2Sampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};


//==============================================================================
//  Helper functions
//==============================================================================

//------------------------------------------------------------------------------
/**
    Scale down pseudo-HDR-value into RGB8.
*/
/*
float4 EncodeHDR(in float4 rgba)
{
    const float colorSpace = 0.8;
    const float maxHDR = 8;
    const float hdrSpace = 1 - colorSpace;
    const float hdrPow = 10;
    const float hdrRt = 0.1;

    float3 col = clamp(rgba.rgb,0,1) * colorSpace;
    float3 hdr = pow(clamp(rgba.rgb,1,10),hdrRt)-1;
    float4 result;
    hdr = clamp(hdr, 0, hdrSpace);
    result.rgb = col + hdr;
    result.a = rgba.a;
    return result;
}
*/

color4 EncodeHDR(in color4 rgba)
{
    return rgba * color4(0.5, 0.5, 0.5, 1.0);
}

//------------------------------------------------------------------------------
/**
    Vertex shader part to compute texture coordinate shadow texture lookups.
*/
float4 vsComputeScreenCoord(float4 pos)
{
    return pos;
}

//------------------------------------------------------------------------------
/**
    Pixel shader part to compute texture coordinate shadow texture lookups.
*/
float2 psComputeScreenCoord(float4 pos)
{
    float2 screenCoord = ((pos.xy / pos.ww) * float2(0.5, -0.5)) + float2(0.5f, 0.5f);
    screenCoord += HalfPixelSize.xy;
    return screenCoord;
}

//------------------------------------------------------------------------------
/**
    Vertex shader part for per-pixel-lighting. Computes the light
    and half vector in tangent space which are then passed to the
    interpolators.
*/
void vsLight(in float3 position,
             in float3 normal,
             in float3 tangent,
             in float3 binormal,
             in float3 modelEyePos,
             in float3 modelLightPos,
             out float3 tangentLightVec,
             out float3 modelLightVec,
             out float3 halfVec,
             out float3 tangentEyePos)
{
    float3 eVec = normalize(modelEyePos - position);
    if (LightType == 0)
    {
        // point light
        modelLightVec = modelLightPos - position;
    }
    else
    {
        // directional light
        modelLightVec = modelLightPos;
    }
    float3 hVec = normalize(normalize(modelLightVec) + eVec);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);
    tangentLightVec = mul(tangentMatrix, modelLightVec);
    halfVec = mul(tangentMatrix, hVec);
    tangentEyePos = normalize(mul(tangentMatrix, eVec));
}

//------------------------------------------------------------------------------
/**
    Compute shadow value. Returns a shadow intensity between 1 (not in shadow)
    and 0 (fully in shadow)
*/
half psShadow(in float2 screenCoord)
{
    // determine if pixel is in shadow
    color4 shadow = tex2D(ShadowSampler, screenCoord);
    half shadowValue = 1.0 - saturate(length(ShadowIndex * shadow));
    return shadowValue;
}

//------------------------------------------------------------------------------
/**
    Compute per-pixel lighting.

    NOTE: lightVec.w contains the distance to the light source
*/
color4 psLight(in color4 mapColor, in float3 tangentSurfaceNormal, in float3 lightVec, in float3 modelLightVec, in float3 halfVec, in half shadowValue)
{
    color4 color = mapColor * color4(LightAmbient.rgb + MatEmissive.rgb * MatEmissiveIntensity, MatDiffuse.a);

    // light intensities
    half specIntensity = pow(saturate(dot(tangentSurfaceNormal, normalize(halfVec))), MatSpecularPower); // Specular-Modulation * mapColor.a;
    half diffIntensity = dot(tangentSurfaceNormal, normalize(lightVec));

    color3 diffColor = mapColor.rgb * LightDiffuse.rgb * MatDiffuse.rgb;
    color3 specColor = specIntensity * LightSpecular.rgb * MatSpecular.rgb;

    // attenuation
    if (LightType == 0)
    {
        // point light source
        diffIntensity *= shadowValue * (1.0f - saturate(length(modelLightVec) / LightRange));
    }
    else
    {
        #if DIRLIGHTS_ENABLEOPPOSITECOLOR
        color.rgb += saturate(-diffIntensity) * DIRLIGHTS_OPPOSITECOLOR * mapColor.rgb * MatDiffuse.rgb;
        #endif
        diffIntensity *= shadowValue;
    }
    color.rgb += saturate(diffIntensity) * (diffColor.rgb + specColor.rgb);
    return color;
}

//------------------------------------------------------------------------------
/**
    Alternative per-pixel lighting, does more stuff per-pixel to reduce
    per-vertex artefacts.
*/
/*
color4 psLight2(in color4 mapColor, in float3 tangentSurfaceNormal, in float3 pos, in float3 normal, in float3 tangent)
{
    // compute tangent space ligth and half vectors
    float3 eVec = ModelEyePos - pos;
    float3 lVec = ModelLightPos - pos;
    float3 hVec = normalize(lVec + eVec);
    float3 binormal = cross(normalize(normal), normalize(tangent));
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);
    float3 lightVec = normalize(mul(tangentMatrix, lVec));
    float3 halfVec  = normalize(mul(tangentMatrix, hVec));

    // compute light intensities
    half diffIntensity = saturate(dot(tangentSurfaceNormal, lightVec));
    half specIntensity = pow(saturate(dot(tangentSurfaceNormal, halfVec)), MatSpecularPower);

    // attenuation
    half att;
    if (LightType == 0)
    {
        // point light source
        att = 1.0f - saturate(length(lVec) / LightRange);
    }
    else
    {
        // directional light source
        att = 1.0f;
    }

    // compute light color
    color4 diffColor = diffIntensity * mapColor * LightDiffuse * MatDiffuse;
    color4 specColor = specIntensity * LightSpecular * MatSpecular * diffIntensity;
    color4 ambColor  = mapColor * LightAmbient * MatAmbient;
    color4 color = color4((att * (diffColor.rgb + specColor.rgb)) + ambColor.rgb, mapColor.a);
    return color;
}
*/

//------------------------------------------------------------------------------
/**
    Compute per-pixel lighting for leafs.
*/
/*
color4 psLightLeaf(in color4 mapColor, in color4 diffuseColor, in float3 lightVec)
{
    // compute light colors
    color4 diffColor = mapColor * diffuseColor;
    color4 ambColor  = mapColor * LightAmbient * MatAmbient;

    // attenuation
    half att;
    if (LightType == 0)
    {
        // point light source
        att = 1.0f - saturate(length(lightVec) / LightRange);
    }
    else
    {
        // directional light source
        att = 1.0f;
    }

    color4 color;
    color.rgb = att * (diffColor + ambColor);
    color.a = mapColor.a;
    return color;
}
*/

//------------------------------------------------------------------------------
/**
    Helper function for tree and leaf vertex shaders which computes
    the swayed vertex position.

    @param  inPosition  input model space position
    @return             swayed model space position
*/
float4 ComputeSwayedPosition(const float4 inPosition)
{
    float ipol = (inPosition.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);
    float4 rotPosition  = float4(mul(Swing, inPosition), 1.0f);
    return lerp(inPosition, rotPosition, ipol);
}

//------------------------------------------------------------------------------
/**
    Helper function for leaf vertex shaders to compute rotated, extruded
    leaf corner vertex in model space.

    @param  inPosition  input model space position
    @param  extrude     the extrude vector
    @return             output model space position
*/
float4 ComputeRotatedExtrudedLeafVertex(const float4 inPosition, const float3 extrude)
{
    float4 position = inPosition;

    // compute rotation angle
    float t = frac((Time / SpriteSwingTime) + position.x);
    float t2 = t * t;
    float t3 = t2 * t;
    float ang = SpriteSwingAngle * t * (GenAngle.x * t3 + GenAngle.y * t2 + GenAngle.z * t + GenAngle.w);

    // build rotation matrix
    float sinAng, cosAng;
    sincos(ang, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    // rotate extrude vector and transform to model space
    float3 extrudeVec = mul(rot, extrude);
    float3 modelExtrudeVec = mul(extrudeVec, (float3x3)InvModelView);

    // extrude to corner position
    position.xyz += modelExtrudeVec.xyz;

    return position;
}
//------------------------------------------------------------------------------
/**
    Returns the uv offset for parallax mapping
*/
half2 ParallaxUv(float2 uv, sampler bumpMap, float3 eyeVect)
{
    float amount = BumpScale / 1000.0f;
    return ((tex2D(bumpMap, uv).a) * amount - amount * 0.5) * eyeVect;
}

//==============================================================================
//  Vertex and pixel shader functions.
//==============================================================================

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry.
*/
vsOutputStaticDepth vsStaticDepth(const vsInputStaticDepth vsIn)
{
    vsOutputStaticDepth vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.depth = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for static geometry with uv
    coordinates.
*/
vsOutputUvDepth vsStaticUvDepth(const vsInputUvDepth vsIn)
{
    vsOutputUvDepth vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0depth.xy = vsIn.uv0;
    vsOut.uv0depth.z = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
color4 psStaticDepth(const vsOutputStaticDepth psIn) : COLOR
{
    return float4(psIn.depth, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate depth values for static geometry with
    alpha test.
*/
color4 psStaticDepthATest(const vsOutputUvDepth psIn) : COLOR
{
    float alpha = tex2D(DiffSampler, psIn.uv0depth.xy).a;
    clip(alpha - (AlphaRef / 256.0f));
    return float4(psIn.uv0depth.z, 0.0f, 0.0f, alpha);
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
*/
vsOutputStaticColor vsStaticColor(const vsInputStaticColor vsIn)
{
    vsOutputStaticColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
    With UV animation.
*/
vsOutputStaticColor vsUVAnimColor(const vsInputStaticColor vsIn)
{
    vsOutputStaticColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);

    // animate uv
    vsOut.uv0 = vsIn.uv0 + (Velocity.xy * Time);

    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
    Modified for Radiosity Parallax Mapping.
    Add the tangentEyePos.
*/
vsOutputRPLStaticColor vsRPLStaticColor(const vsInputStaticColor vsIn)
{
     vsOutputRPLStaticColor vsOut;
     vsOut.position = mul(vsIn.position, ModelViewProjection);

    // The x axis of the UV-coordinates needs to be switched for parallaxmapping

    float2 tmpUv = vsIn.uv0;
    tmpUv.x = 1- tmpUv.x;
    vsOut.uv0 = tmpUv;

    //vsOut.uv0 = vsIn.uv0;

    // Compute the binormal and the tangentMatrix
    float3 binormal = cross(vsIn.normal, vsIn.tangent);
    float3x3 mytangentMatrix = float3x3(vsIn.tangent, binormal, vsIn.normal);

    // The ViewVector is needed for Parallaxmapping
    float3 ePos = ModelEyePos - vsIn.position;
    // Compute the Eyeposition in tangentspace
    VS_SETSCREENPOS(vsOut.position);
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent,vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate color values for static geometry.
*/
color4 psStaticColor(const vsOutputStaticColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;

    color4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
};

//------------------------------------------------------------------------------
/**
    Vertex shader: generate vertices for soft silhouettes.
*/
vsOutputSoftSilhouette vsSoftSilhouette(const vsInputStaticColor vsIn)
{
    vsOutputSoftSilhouette vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0silhouette.xy = vsIn.uv0;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent,vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);

    // compute silhouette modulation color
    float3 eVec = normalize(ModelEyePos - vsIn.position);
    vsOut.uv0silhouette.z = abs(dot(vsIn.normal, eVec));
    vsOut.uv0silhouette.z *= vsOut.uv0silhouette.z;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate color values for soft silhouette geometry
*/
color4 psSoftSilhouette(const vsOutputSoftSilhouette psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0silhouette.xy, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0silhouette.xy + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0silhouette.xy + uvOffset).rgb * 2.0f) - 1.0f;

    color4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    baseColor *= psIn.uv0silhouette.z;
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate color values for static geometry.
    Modified for Radiosity Parallax Lightmapping
*/
color4 psRPLStaticColor(const vsOutputRPLStaticColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = float2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;
    color4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.halfVec, psIn.modelLightVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for environment mapping effect.
*/
vsOutputEnvironmentColor vsEnvironmentColor(const vsInputStaticColor vsIn)
{
    vsOutputEnvironmentColor vsOut;
    vsOut.position  = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);

    // compute a model space reflection vector
    float3 modelEyeVec = normalize(vsIn.position.xyz - ModelEyePos);
    float3 modelReflect = reflect(modelEyeVec, vsIn.normal);

    // transform model space reflection vector to view space
    vsOut.worldReflect = mul(modelReflect, (float3x3)Model);

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for environment mapping effect.
*/
color4 psEnvironmentColor(const vsOutputEnvironmentColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;
    color4 reflectColor = tex2D(EnvironmentSampler, psIn.worldReflect);
    color4 color = lerp(diffColor, reflectColor, diffColor.a);
    color4 finalColor = psLight(color, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(finalColor);
    }
    else
    {
        return finalColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for lightmapping. Note: the current implementation
    fakes dynamic lighting, light always shines vertically onto the
    surface!
*/
vsOutputLightmappedColor vsLightmappedColor(const vsInputLightmappedColor vsIn)
{
    vsOutputLightmappedColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv1 = vsIn.uv1;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Alternative vertex shader for lightmapping.
*/
vsOutputStatic2Color vsLightmappedColor2(const vsInputStatic2Color vsIn)
{
    vsOutputStatic2Color vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv1 = vsIn.uv1;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent,vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Alternative pixel shader for lightmapping.
*/
color4 psLightmappedColor2(const vsOutputStatic2Color psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    color4 lmColor   = tex2D(Diff1Sampler, psIn.uv1);
    diffColor.rgb *= lmColor.rgb * Intensity0;
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;
    color4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for Radiosity Normalmapping.
*/
vsOutputRNLightmappedColor vsRNLightmappedColor(const vsInputLightmappedColor vsIn)
{
    vsOutputRNLightmappedColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);

    // The x axis of the UV-coordinates needs to be switched for parallaxmapping
    float2 tmpUv = vsIn.uv0;
    tmpUv.x = 1- tmpUv.x;
    vsOut.uv0 = tmpUv;
    vsOut.uv1 = vsIn.uv1;

    // Compute the binormal and the tangentMatrix
    float3 binormal = cross(vsIn.normal, vsIn.tangent);
    float3x3 mytangentMatrix = float3x3(vsIn.tangent, binormal, vsIn.normal);

    // The ViewVector is needed for Parallaxmapping
    float3 ePos = ModelEyePos - vsIn.position;
    // Compute the Eyeposition in tangentspace
    vsOut.eyePos = normalize(mul(mytangentMatrix, normalize(ePos)));

    // The Screenpos for debuging
    VS_SETSCREENPOS(vsOut.position);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for skinning (write depth values).
*/
vsOutputStaticDepth vsSkinnedDepth(const vsInputSkinnedDepth vsIn)
{
    vsOutputStaticDepth vsOut;
    float4 skinPos = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    vsOut.position = mul(skinPos, ModelViewProjection);
    vsOut.depth = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for skinning (write color values).
*/
vsOutputStaticColor vsSkinnedColor(const vsInputSkinnedColor vsIn)
{
    vsOutputStaticColor vsOut;

    // get skinned position, normal and tangent
    float4 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    vsOut.position     = mul(skinPos, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0          = vsIn.uv0;
    float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinTangent = skinnedNormal(vsIn.tangent, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinBinormal = skinnedNormal(vsIn.binormal, vsIn.weights, vsIn.indices, JointPalette);
    vsLight(skinPos, skinNormal, skinTangent, skinBinormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for lightmapping. Note: dynamic lighting will be added
    with conventional lighting passes afterwards!
*/
color4 psLightmappedColor(const vsOutputLightmappedColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    color4 diffColor = tex2D(DiffSampler, psIn.uv0);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0).rgb * 2.0f) - 1.0f;
    color4 baseColor = tex2D(LightmapSampler, psIn.uv1) * Intensity0 * diffColor * tangentSurfaceNormal.z;
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Pixel shader for Radiosity Normalmapping.
*/
color4 psRNLightmappedColor(const vsOutputRNLightmappedColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    // The three Axes of our Bumpbasis for Radiosity Parallax Lightmapping
    // with switched y and z Axis!!!

     float3 bumpBasisX =float3(0.70710678118654752440084436210485
                              , 0
                              , 0.57735026918962576450914878050196);
     float3 bumpBasisY =float3(-0.40824829046386301636621401245098
                              ,-0.70710678118654752440084436210485
                              , 0.57735026918962576450914878050196);
     float3 bumpBasisZ =float3( -0.40824829046386301636621401245098
                              , 0.70710678118654752440084436210485
                              , 0.57735026918962576450914878050196  );

    float2 uvOffset = float2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {   // Compute the new uv Coordinates based on the Heightmap.
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }

    // The Diffuse Texture
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);

    // The LightmapColors for the three Axes
    color4 lightmapColorX =     tex2D(RadiosityNMSampler1, psIn.uv1);
    color4 lightmapColorY =     tex2D(RadiosityNMSampler2, psIn.uv1);
    color4 lightmapColorZ =     tex2D(RadiosityNMSampler3, psIn.uv1);

    // The Surface-Normal in our bump basis for the radiosity normalmapping
    float3 bumpSurfaceNormal= (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f)-1.0f;

    bumpSurfaceNormal.x = -bumpSurfaceNormal.x;
    bumpSurfaceNormal.y = -bumpSurfaceNormal.y;


    // The main calculation for the radiosity normal mapping
    float3 BumpFactor;
    BumpFactor.x = saturate(dot(bumpSurfaceNormal, bumpBasisX ));
    BumpFactor.y = saturate(dot(bumpSurfaceNormal, bumpBasisY ));
    BumpFactor.z = saturate(dot(bumpSurfaceNormal, bumpBasisZ ));

    color4 lightmapColor =  (pow(BumpFactor.x,1)) * lightmapColorX +
                            (pow(BumpFactor.y,1)) * lightmapColorY +
                            (pow(BumpFactor.z,1)) * lightmapColorZ;

    // The Map with the pure Radiosity
    color4 radiosityColor = tex2D(RadiosityNMSampler4, psIn.uv1);
    //return (lightmapColor+radiosityColor)*diffColor;
    if (hdr)
    {
        return EncodeHDR(lightmapColor+radiosityColor)*diffColor;
    }
    else
    {
        return (lightmapColor+radiosityColor)*diffColor;
    }
    //return (lightmapColor+radiosityColor)*diffColor;
    // Without Radiosity
    //return (lightmapColor)*diffColor;
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for blending (depth values).
*/
vsOutputStaticDepth vsBlendedDepth(const vsInputBlendedDepth vsIn)
{
    vsOutputStaticDepth vsOut;
    float4 pos = 0;
    if (VertexStreams > 0)
    {
        pos = vsIn.position0;
        if (VertexStreams > 1)
        {
            pos += (vsIn.position1-vsIn.position0) * VertexWeights1.y;
            if (VertexStreams > 2)
            {
                pos += (vsIn.position2-vsIn.position0) * VertexWeights1.z;
                if (VertexStreams > 3)
                {
                    pos += (vsIn.position3-vsIn.position0) * VertexWeights1.w;
                    if (VertexStreams > 4)
                    {
                        pos += (vsIn.position4-vsIn.position0) * VertexWeights2.x;
                        if (VertexStreams > 5)
                        {
                            pos += (vsIn.position5-vsIn.position0) * VertexWeights2.y;
                            if (VertexStreams > 6)
                            {
                                pos += (vsIn.position6-vsIn.position0) * VertexWeights2.z;
                            }
                        }
                    }
                }
            }
        }
    }
    vsOut.position = mul(pos, ModelViewProjection);
    vsOut.depth = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for blending (color values).
*/
vsOutputStaticColor vsBlendedColor(const vsInputBlendedColor vsIn)
{
    vsOutputStaticColor vsOut;
    float4 pos = 0;
    float3 normal = 0;
    float3 tangent = 0;
    if (VertexStreams > 0)
    {
        pos = vsIn.position0;
        normal = vsIn.normal0;
        if (VertexStreams > 1)
        {
            pos += (vsIn.position1 - vsIn.position0) * VertexWeights1.y;
            normal += (vsIn.normal1 - vsIn.normal0) * VertexWeights1.y;
            if (VertexStreams > 2)
            {
                pos += (vsIn.position2 - vsIn.position0) * VertexWeights1.z;
                normal += (vsIn.normal2 - vsIn.normal0) * VertexWeights1.z;
                if (VertexStreams > 3)
                {
                    pos += (vsIn.position3 - vsIn.position0) * VertexWeights1.w;
                    normal += (vsIn.normal3 - vsIn.normal0) * VertexWeights1.w;
                    if (VertexStreams > 4)
                    {
                        pos += (vsIn.position4 - vsIn.position0) * VertexWeights2.x;
                        normal += (vsIn.normal4 - vsIn.normal0) * VertexWeights2.x;
                        if (VertexStreams > 5)
                        {
                            pos += (vsIn.position5 - vsIn.position0) * VertexWeights2.y;
                            normal += (vsIn.normal5 - vsIn.normal0) * VertexWeights2.y;
                            if (VertexStreams > 6)
                            {
                                pos += (vsIn.position6 - vsIn.position0) * VertexWeights2.z;
                                normal += (vsIn.normal6 - vsIn.normal0) * VertexWeights2.z;
                            }
                        }
                    }
                }
            }
        }
    }
    vsOut.position = mul(pos, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsLight(pos, normal, vsIn.tangent, cross(normal,vsIn.tangent), ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for alpha blended environment mapped geometry.
*/
color4 psEnvironmentAlphaColor(const vsOutputEnvironmentColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;
    color4 reflectColor = tex2D(EnvironmentSampler, psIn.worldReflect);

    // FIXME: this throws a compiler error that the target and source
    // of a lerp() cannot be the same...
    //color4 color = lerp(diffColor, reflectColor, diffColor.a);
    color4 color = diffColor;

    color.a = diffColor.a;
    color4 baseColor = psLight(color, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for alpha blended geometry with 2 alpha layers.
*/
vsOutputStatic2Color vsStaticAlpha2Color(const vsInputStatic2Color vsIn)
{
    vsOutputStatic2Color vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = mul(vsIn.uv0, TextureTransform0);
    vsOut.uv1 = mul(vsIn.uv1, TextureTransform1);
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    VS_SETSCREENPOS(vsOut.position);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for alpha blended geometry with 2 alpha layers.
*/
color4 psStaticAlpha2Color(const vsOutputStatic2Color psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0, BumpSampler, psIn.eyePos);
    }
    color4 diffColor0 = tex2D(DiffSampler, psIn.uv0 + uvOffset);
    color4 diffColor1 = tex2D(Diff1Sampler, psIn.uv1 + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0 + uvOffset).rgb * 2.0f) - 1.0f;
    color4 diffColor;
    diffColor = lerp(diffColor0, diffColor1, diffColor0.a);
    color4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for terrain color.
*/
vsOutputTerrainColor vsTerrainColor(const vsInputTerrainColor vsIn)
{
    vsOutputTerrainColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);

    // generate texture coordinates after OpenGL rules
    vsOut.uv0.x = dot(vsIn.position, TexGenS);
    vsOut.uv0.y = dot(vsIn.position, TexGenT);
    vsOut.uv1.xy = vsIn.position.xz * TexScale;
    vsOut.uv2.xy = vsIn.position.xz * DetailTexScale;
    float eyeDist = distance(ModelEyePos, vsIn.position);
    vsOut.fog.x = 1.0f - saturate(eyeDist / DetailEnd);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for terrain color.
*/
color4 psTerrainColor(const vsOutputTerrainColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05, 0.0f, 0.0f, 1.0f);
#else
    // sample material weight texture
    color4 matWeights = tex2D(WeightSampler, psIn.uv0);

    // sample tile textures
    color4 baseColor = matWeights.x * lerp(tex2D(GrassSampler, psIn.uv1), tex2D(GrassSampler, psIn.uv2), psIn.fog.x);
    baseColor += matWeights.y * lerp(tex2D(RockSampler, psIn.uv1), tex2D(RockSampler, psIn.uv2), psIn.fog.x);
    baseColor += matWeights.z * lerp(tex2D(GroundSampler, psIn.uv1), tex2D(GroundSampler, psIn.uv2), psIn.fog.x);
    baseColor += matWeights.w * SnowAmplify * tex2D(SnowSampler, psIn.uv1);

    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate depth values for leafs.
*/
vsOutputUvDepth vsLeafDepth(const vsInputLeafDepth vsIn)
{
    vsOutputUvDepth vsOut;

    // compute swayed position in model space
    float4 position = ComputeSwayedPosition(vsIn.position);
    position = ComputeRotatedExtrudedLeafVertex(position, vsIn.extrude);

    // transform to projection space
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0depth.xy = vsIn.uv0;
    vsOut.uv0depth.z  = vsOut.position.z;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for leafs.
*/
vsOutputLeafColor vsLeafColor(const vsInputLeafColor vsIn)
{
    vsOutputLeafColor vsOut;

    // compute swayed position in model space
    float4 position = ComputeSwayedPosition(vsIn.position);
    position = ComputeRotatedExtrudedLeafVertex(position, vsIn.extrude);

    // transform to projection space
    vsOut.position = mul(position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0intensity.xy = vsIn.uv0;

    // compute light vec
    float3 lightVec = ModelLightPos - position;

    // compute a model space normal and tangent vector
    float3 posCenter = vsIn.position - BoxCenter;
    float3 normal   = normalize(posCenter);
    float3 tangent  = cross(normal, float3(0.0f, 1.0f, 0.0f));
    float3 binormal = cross(normal, tangent);

    // compute a selfshadowing value
    float relDistToCenter = dot(posCenter, posCenter) / dot(BoxCenter-BoxMaxPos*0.8f, BoxCenter-BoxMaxPos*0.8f);
    float selfShadow = lerp(InnerLightIntensity, OuterLightIntensity, relDistToCenter);
    vsOut.uv0intensity.z = selfShadow;

    vsLight(vsIn.position, normal, tangent, binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Color pixel shader for leafs.
*/
color4 psLeafColor(const vsOutputLeafColor psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else

    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;
    half2 uvOffset = half2(0.0f, 0.0f);
    if (BumpScale != 0.0f)
    {
        uvOffset = ParallaxUv(psIn.uv0intensity.xy, BumpSampler, psIn.eyePos);
    }
    color4 diffColor = tex2D(DiffSampler, psIn.uv0intensity.xy + uvOffset);
    float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv0intensity.xy + uvOffset).rgb * 2.0f) - 1.0f;

    color4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    baseColor *= psIn.uv0intensity.z;
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }


#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for tree pixel depth.
*/
vsOutputUvDepth vsTreeDepth(const vsInputUvDepth vsIn)
{
    vsOutputUvDepth vsOut;

    float4 position = ComputeSwayedPosition(vsIn.position);
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0depth.xy = vsIn.uv0;
    vsOut.uv0depth.z  = vsOut.position.z;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for tree pixel color.
*/
vsOutputStaticColor vsTreeColor(const vsInputStaticColor vsIn)
{
    vsOutputStaticColor vsOut;

    // compute lerp factor based on height above min y
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);

    // compute rotated vertex position, normal and tangent in model space
    float4 rotPosition  = float4(mul(Swing, vsIn.position), 1.0f);
    float3 rotNormal    = mul(Swing, vsIn.normal);
    float3 rotTangent   = mul(Swing, vsIn.tangent);

    // lerp between original and rotated pos
    float4 lerpPosition = lerp(vsIn.position, rotPosition, ipol);
    float3 lerpNormal   = lerp(vsIn.normal, rotNormal, ipol);
    float3 lerpTangent  = lerp(vsIn.tangent, rotTangent, ipol);

    // transform vertex position
    vsOut.position = transformStatic(lerpPosition, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0 = vsIn.uv0;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for grass.
*/
vsOutputSpriteColor vsGrassColor(const vsInputStaticColor vsIn)
{
    vsOutputSpriteColor vsOut;

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
    float3 lightVec = normalize(ModelLightPos - vsIn.position);
    float dotNL = dot(vsIn.normal, lightVec);
    float primDiffIntensity = saturate(dotNL);
    vsOut.color = MatAmbient + (MatDiffuse * (LightDiffuse * primDiffIntensity));
    vsOut.color.a = saturate((MinDist - dist) / (MaxDist - MinDist));
    vsOut.uvx.xy = frac(vsIn.uv0);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for lightmapped grass.
*/
vsOutputSpriteLightmappedColor vsGrassLightmappedColor(const vsInputLightmappedColor vsIn)
{
    vsOutputSpriteLightmappedColor vsOut;

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
    float3 lightVec = normalize(ModelLightPos - vsIn.position);
    float dotNL = dot(vsIn.normal, lightVec);
    float primDiffIntensity = saturate(dotNL);
    vsOut.color = MatAmbient + (MatDiffuse * (LightDiffuse * primDiffIntensity));
    vsOut.color.a = saturate((MinDist - dist) / (MaxDist - MinDist));
    vsOut.uvx.xy = frac(vsIn.uv0);
    vsOut.uvx.zw = vsIn.uv1;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for grass.
*/
color4 psGrassColor(const psInputSpriteColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else
    color4 spriteColor = tex2D(SpriteSampler, psIn.uv0);
    color4 baseColor = tex2D(DiffSampler, psIn.uvx.xy);
    color4 finalColor = spriteColor * baseColor * psIn.color;
    if (hdr)
    {
        return EncodeHDR(finalColor);
    }
    else
    {
        return finalColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Pixel shader for lightmapped grass.
*/
color4 psGrassLightmappedColor(const psInputSpriteLightmappedColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else
    color4 spriteColor = tex2D(SpriteSampler, psIn.uv0);
    color4 lightmapColor = tex2D(LightmapSampler, psIn.uvx.zw);
    color4 baseColor = tex2D(DiffSampler, psIn.uvx.xy);
    color4 finalColor = spriteColor * baseColor * lightmapColor * psIn.color * LightDiffuse * Intensity0;
    if (hdr)
    {
        return EncodeHDR(finalColor);
    }
    else
    {
        return finalColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for cubemapped sky box.
*/
vsOutputCubeSkyboxColor vsCubeSkyboxColor(vsInputCubeSkyboxColor vsIn)
{
    vsOutputCubeSkyboxColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0      = mul(vsIn.position, (float3x3)Model);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for cubemapped sky box.
*/
color4 psCubeSkyboxColor(vsOutputCubeSkyboxColor psIn, uniform bool hdr) : COLOR
{
    color4 baseColor = texCUBE(EnvironmentSampler, psIn.uv0) * MatDiffuse;
    if (hdr)
    {
        return EncodeHDR(baseColor);
    }
    else
    {
        return baseColor;
    }
}

//------------------------------------------------------------------------------
/**
    Vertex shader for sky box.
*/
vsOutputSkyboxColor vsSkyboxColor(vsInputSkyboxColor vsIn)
{
    vsOutputSkyboxColor vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for sky box.
*/
color4 psSkyboxColor(vsOutputSkyboxColor psIn, uniform bool hdr) : COLOR
{
    color4 color = tex2D(SkySampler, psIn.uv0) * MatDiffuse * Intensity0;
    if (hdr)
    {
        return EncodeHDR(color);
    }
    else
    {
        return color;
    }
}

//------------------------------------------------------------------------------
/**
    Vertex shader for particles.
*/
vsOutputParticleColor vsParticleColor(const vsInputParticleColor vsIn)
{
    float rotation = vsIn.transform[0];
    float size     = vsIn.transform[1];

    // build rotation matrix
    float sinAng, cosAng;
    sincos(rotation, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    float4 position =  vsIn.position;
    float3 extrude  =  float3(vsIn.extrude, 0.0f);
    extrude *= size;
    extrude =  mul(rot, extrude);
    extrude =  mul(extrude, (float3x3) InvModelView);
    position.xyz += extrude.xyz;

    vsOutputParticleColor vsOut;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0      = vsIn.uv0;
    vsOut.diffuse  = vsIn.color;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for particle2's.
*/
vsOutputParticle2Color vsParticle2Color(const vsInputParticle2Color vsIn)
{
    float code     = vsIn.data[0];
    float rotation = vsIn.data[1];
    float size     = vsIn.data[2];
    float colorCode  = vsIn.data[3];

    // build rotation matrix
    float sinAng, cosAng;
    sincos(rotation, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    // decode color data
    float4  rgba;
    rgba.z = modf(colorCode/256.0f,colorCode);
    rgba.y = modf(colorCode/256.0f,colorCode);
    rgba.x = modf(colorCode/256.0f,colorCode);
    rgba.w = modf(code/256.0f,code);
    rgba *= 256.0f/255.0f;

    float4 position =  vsIn.position;


    // the corner offset gets calculated from the velocity

    float3 extrude = mul(InvModelView,vsIn.velocity);
    if(code != 0.0f)
    {
        extrude = normalize(extrude);
        float vis = abs(extrude.z);
        size *= cos(vis*3.14159f*0.5f);
        rgba.w *= cos(vis*3.14159f*0.5f);
    };
    extrude.z = 0.0f;
    extrude = normalize(extrude);

    extrude *= size;
    extrude =  mul(rot, extrude);
    extrude =  mul(extrude, (float3x3) InvModelView);
    position.xyz += extrude.xyz;

    vsOutputParticle2Color vsOut;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0      = vsIn.uv;
    vsOut.diffuse  = rgba;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader for particles.
*/
color4 psParticleColor(const vsOutputParticleColor psIn, uniform bool hdr) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else
    color4 finalColor = tex2D(DiffSampler, psIn.uv0) * psIn.diffuse;
    if (hdr)
    {
        return EncodeHDR(finalColor);
    }
    else
    {
        return finalColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Vertex shader for 4-layered shader.
*/
vsOutputStatic2Color vsLayeredColor(const vsInputStatic2Color vsIn)
{
    vsOutputStatic2Color vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0      = mul(vsIn.uv0, DetailTexture);
    vsOut.uv1      = vsIn.uv0;
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for multi-layered shader.
*/
vsOutputStatic3Color vsMultiLayeredColor(const vsInputStatic3Color vsIn)
{
    vsOutputStatic3Color vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    VS_SETSCREENPOS(vsOut.position);
    vsOut.uv0      = vsIn.uv0;
    vsOut.uv1      = vsIn.uv0 * MLPUVStretch[0][0];
    vsOut.uv2      = vsIn.uv0 * MLPUVStretch[0][1];
    vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
    return vsOut;
}


//------------------------------------------------------------------------------
/**
    Pixel shader for 4-layered shader.
*/
color4 psLayeredColor(const vsOutputStatic2Color psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;

    // read tiled material colors
    color4 material0 = tex2D(DiffSampler,  psIn.uv0);
    color4 material1 = tex2D(Diff1Sampler, psIn.uv0);
    color4 material2 = tex2D(Diff2Sampler, psIn.uv0);
    color4 material3 = tex2D(Diff3Sampler, psIn.uv0);

    // read weight colors
    half weight0 = tex2D(Layer0Sampler, psIn.uv1).r;
    half weight1 = tex2D(Layer1Sampler, psIn.uv1).r;
    half weight2 = tex2D(Layer2Sampler, psIn.uv1).r;

    color4 color = lerp(material0, material1, weight0);
    color = lerp(color, material2, weight1);
    color = lerp(color, material3, weight2);
    color4 finalColor = psLight(color, float3(0.0f, 0.0f, 1.0f), psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);
    if (hdr)
    {
        return EncodeHDR(finalColor);
    }
    else
    {
        return finalColor;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Pixel shader for Multi-layered shader.
*/
color4 psMultiLayeredColor(const vsOutputStatic3Color psIn, uniform bool hdr, uniform bool shadow) : COLOR
{
#if DEBUG_LIGHTCOMPLEXITY
    return float4(0.05f, 0.0f, 0.0f, 1.0f);
#else
    half shadowIntensity = shadow ? psShadow(PS_SCREENPOS) : 1.0;

    // read tiled material colors
    color4 material0 = tex2D(Diff2Sampler, psIn.uv1);
    color4 material1 = tex2D(Diff3Sampler, psIn.uv2);
    color4 material2 = tex2D(Layer0Sampler,psIn.uv0 * MLPUVStretch[0][2]);
    color4 material3 = tex2D(Layer1Sampler,psIn.uv0 * MLPUVStretch[0][3]);
    color4 material4 = tex2D(Layer2Sampler,psIn.uv0 * MLPUVStretch[1][0]);

    // read weight colors, rgb of trans1 is the lightmap
    float4 trans1 = tex2D(DiffSampler, psIn.uv0);
    float4 trans2 = tex2D(Diff1Sampler, psIn.uv0);

    half weight0 = trans1.w*material0.w;
    half weight1 = trans2.r*material1.w;
    half weight2 = trans2.g*material2.w;
    half weight3 = trans2.b*material2.w;
    half weight4 = trans2.w*material3.w;

    material0.rgb *= weight0;

    color4 color    = lerp(material0, material1, weight1);
    color           = lerp(color,material2, weight2);
    color           = lerp(color,material3, weight3);
    color           = lerp(color,material4, weight4);

    color.rgb *= trans1.rgb;
    color4 finalColor = psLight(color, float3(0.0f, 0.0f, 1.0f), psIn.lightVec, psIn.modelLightVec, psIn.halfVec, shadowIntensity);

    // the alpha channel of the transparency masks = (1-r)*(1-g)*(1-b)   , needed here for final blending
    // the following line is equal to : <finalColor.w = (1-weight0)*(1-weight1)*(1-weight2)*(1-weight3)*(1-weight4);>
    // the only difference is that it is 4 instructions smaller, so that the whole shader gets <=64 instructions
    finalColor.w = lerp(lerp(lerp(lerp((1-weight4),0,weight3),0,weight2),0,weight1),0,weight0);

    if (hdr)
    {
        return EncodeHDR(finalColor);
    }
    else
    {
        return finalColor;
    }
#endif
}


//------------------------------------------------------------------------------
/**
    Vertex shader function for final image composing.
*/
vsOutputCompose vsCompose(const vsInputCompose vsIn)
{
    vsOutputCompose vsOut;
    vsOut.position = vsIn.position;
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader function for final image composing.
*/
color4 psCompose(const vsOutputCompose psIn) : COLOR
{
    color4 imageColor = tex2D(ComposeSourceBuffer, psIn.uv0);
    half luminance = dot(imageColor.xyz, MatAmbient.xyz);
    return MatDiffuse * lerp(color4(luminance, luminance, luminance, luminance), imageColor, Intensity0);
}

//------------------------------------------------------------------------------
/**
    Vertex shader: process vertices for 3d gui. This will be a orthogonal
    projection.
*/
vsOutputCompose vsGui3D(const vsInputCompose vsIn)
{
    vsOutputCompose vsOut;
    vsOut.position = mul(mul(vsIn.position, Model), OrthoProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Pixel shader: generate color values for static geometry.
*/
color4 psGui3D(const vsOutputCompose psIn) : COLOR
{
    color4 color = tex2D(DiffSampler, psIn.uv0);
    color.rgb *= (MatDiffuse.rgb + MatEmissive.rgb * MatEmissiveIntensity);
    return color;
}

//------------------------------------------------------------------------------
//  Pixel shader for final reflection/refraction water composition with depth in alphachannel.
//------------------------------------------------------------------------------
float4 psWaterDepthCompose(const vsOutputCompose psIn) : COLOR
{
    float4 scene = tex2D(ComposeSourceBuffer, psIn.uv0);
    float4 depth = tex2D(ComposeSourceBuffer1, psIn.uv0);
    float clampedDepth = clamp(depth.r / MaxWaterDist, 0, 1);

    return float4(scene.r, scene.g, scene.b, clampedDepth);
}


//------------------------------------------------------------------------------
/**
    Techniques for shader "static"
*/
technique tStaticDepth
{
    pass p0
    {
        CullMode     = <CullMode>;
        VertexShader = compile VS_PROFILE vsStaticDepth();
        PixelShader  = compile PS_PROFILE psStaticDepth();
    }
}

technique tStaticColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tStaticColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tStaticColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tStaticColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "static_atest".
*/
technique tStaticDepthATest
{
    pass p0
    {
        CullMode     = <CullMode>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsStaticUvDepth();
        PixelShader  = compile PS_PROFILE psStaticDepthATest();
    }
}

technique tStaticColorATest
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tStaticColorATestHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tStaticColorATestShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tStaticColorATestHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "environment"
*/
technique tEnvironmentColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentColor(false, false);
    }
}

technique tEnvironmentColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentColor(true, false);
    }
}

technique tEnvironmentColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentColor(false, true);
    }
}

technique tEnvironmentColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "lightmapped"
*/
technique tLightmappedBaseColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLightmappedColor();
        PixelShader  = compile PS_PROFILE psLightmappedColor(false);
    }
}

technique tLightmappedBaseColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLightmappedColor();
        PixelShader  = compile PS_PROFILE psLightmappedColor(true);
    }
}

technique tLightmappedLitColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLightmappedColor2();
        PixelShader  = compile PS_PROFILE psLightmappedColor2(false, false);
    }
}

technique tLightmappedLitColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLightmappedColor2();
        PixelShader  = compile PS_PROFILE psLightmappedColor2(true, false);
    }
}

technique tLightmappedLitColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLightmappedColor2();
        PixelShader  = compile PS_PROFILE psLightmappedColor2(false, true);
    }
}

technique tLightmappedLitColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLightmappedColor2();
        PixelShader  = compile PS_PROFILE psLightmappedColor2(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "Radiosity Normalmapped"
*/
technique tRNLightmappedBaseColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsRNLightmappedColor();
        PixelShader  = compile PS_PROFILE psRNLightmappedColor(false);
    }
}

technique tRNLightmappedBaseColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsRNLightmappedColor();
        PixelShader  = compile PS_PROFILE psRNLightmappedColor(true);
    }
}

technique tRNLightmappedLitColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsRPLStaticColor();
        PixelShader  = compile PS_PROFILE psRPLStaticColor(false, false);
    }
}

technique tRNLightmappedLitColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsRPLStaticColor();
        PixelShader  = compile PS_PROFILE psRPLStaticColor(true, false);
    }
}

technique tRNLightmappedLitColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsRPLStaticColor();
        PixelShader  = compile PS_PROFILE psRPLStaticColor(false, true);
    }
}

technique tRNLightmappedLitColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsRPLStaticColor();
        PixelShader  = compile PS_PROFILE psRPLStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "skinned"
*/
technique tSkinnedDepth
{
    pass p0
    {
        CullMode = <CullMode>;
        VertexShader = compile VS_PROFILE vsSkinnedDepth();
        PixelShader  = compile PS_PROFILE psStaticDepth();
    }
}

technique tSkinnedColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tSkinnedColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tSkinnedColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tSkinnedColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "skinned alpha"
*/
technique tSkinnedAlphaColor
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tSkinnedAlphaColorHDR
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tSkinnedAlphaColorShadow
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tSkinnedAlphaColorHDRShadow
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSkinnedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "blended"
*/
technique tBlendedDepth
{
    pass p0
    {
        CullMode = <CullMode>;
        VertexShader = compile VS_PROFILE vsBlendedDepth();
        PixelShader  = compile PS_PROFILE psStaticDepth();
    }
}

technique tBlendedColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBlendedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tBlendedColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBlendedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tBlendedColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBlendedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tBlendedColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsBlendedColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "alpha"
*/
technique tStaticAlphaColor
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tStaticAlphaColorHDR
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tStaticAlphaColorShadow
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tStaticAlphaColorHDRShadow
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    UV Animation
*/
technique tUVAnimationAlpha
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsUVAnimColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "alpha"
*/
technique tSoftAlphaColor
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSoftSilhouette();
        PixelShader  = compile PS_PROFILE psSoftSilhouette(false, false);
    }
}

technique tSoftAlphaColorHDR
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSoftSilhouette();
        PixelShader  = compile PS_PROFILE psSoftSilhouette(true, false);
    }
}

technique tSoftAlphaColorShadow
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSoftSilhouette();
        PixelShader  = compile PS_PROFILE psSoftSilhouette(false, true);
    }
}

technique tSoftAlphaColorHDRShadow
{
    pass p0
    {
        CullMode     = <CullMode>;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsSoftSilhouette();
        PixelShader  = compile PS_PROFILE psSoftSilhouette(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "alpha2"
*/
technique tStaticAlpha2Color
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticAlpha2Color();
        PixelShader  = compile PS_PROFILE psStaticAlpha2Color(false, false);
    }
}

technique tStaticAlpha2ColorHDR
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticAlpha2Color();
        PixelShader  = compile PS_PROFILE psStaticAlpha2Color(true, false);
    }
}

technique tStaticAlpha2ColorShadow
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticAlpha2Color();
        PixelShader  = compile PS_PROFILE psStaticAlpha2Color(false, true);
    }
}

technique tStaticAlpha2ColorHDRShadow
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsStaticAlpha2Color();
        PixelShader  = compile PS_PROFILE psStaticAlpha2Color(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "environment_alpha"
*/
technique tEnvironmentAlphaColor
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentAlphaColor(false, false);
    }
}

technique tEnvironmentAlphaColorHDR
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentAlphaColor(true, false);
    }
}

technique tEnvironmentAlphaColorShadow
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentAlphaColor(false, true);
    }
}

technique tEnvironmentAlphaColorHDRShadow
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;
        VertexShader = compile VS_PROFILE vsEnvironmentColor();
        PixelShader  = compile PS_PROFILE psEnvironmentAlphaColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "terrain"
*/
technique tTerrainColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsTerrainColor();
        PixelShader  = compile PS_PROFILE psTerrainColor(false);
    }
}

technique tTerrainColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsTerrainColor();
        PixelShader  = compile PS_PROFILE psTerrainColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for leaf renderer.
*/
technique tLeafDepth
{
    pass p0
    {
        CullMode = None;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsLeafDepth();
        PixelShader  = compile PS_PROFILE psStaticDepthATest();
    }
}

technique tLeafColor
{
    pass p0
    {
        CullMode = None;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsLeafColor();
        PixelShader  = compile PS_PROFILE psLeafColor(false, true);
    }
}

technique tLeafColorHDR
{
    pass p0
    {
        CullMode = None;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsLeafColor();
        PixelShader  = compile PS_PROFILE psLeafColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for tree renderer.
*/
technique tTreeDepth
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsTreeDepth();
        PixelShader  = compile PS_PROFILE psStaticDepthATest();
    }
}

technique tTreeColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsTreeColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, false);
    }
}

technique tTreeColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsTreeColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, false);
    }
}

technique tTreeColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsTreeColor();
        PixelShader  = compile PS_PROFILE psStaticColor(false, true);
    }
}

technique tTreeColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        AlphaRef     = <AlphaRef>;
        VertexShader = compile VS_PROFILE vsTreeColor();
        PixelShader  = compile PS_PROFILE psStaticColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for grass renderer.
*/
technique tGrassColor
{
    pass p0
    {
        CullMode = None;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile VS_PROFILE vsGrassColor();
        PixelShader  = compile PS_PROFILE psGrassColor(false);
    }
}

technique tGrassColorHDR
{
    pass p0
    {
        CullMode = None;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile VS_PROFILE vsGrassColor();
        PixelShader  = compile PS_PROFILE psGrassColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for lightmapped grass renderer.
*/
technique tGrassLightmappedColor
{
    pass p0
    {
        CullMode = None;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile VS_PROFILE vsGrassLightmappedColor();
        PixelShader  = compile PS_PROFILE psGrassLightmappedColor(false);
    }
}

technique tGrassLightmappedColorHDR
{
    pass p0
    {
        CullMode = None;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile VS_PROFILE vsGrassLightmappedColor();
        PixelShader  = compile PS_PROFILE psGrassLightmappedColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for cubemapped sky box.
*/
technique tCubeSkyboxColor
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile VS_PROFILE vsCubeSkyboxColor();
        PixelShader  = compile PS_PROFILE psCubeSkyboxColor(false);
    }
}

technique tCubeSkyboxColorHDR
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile VS_PROFILE vsCubeSkyboxColor();
        PixelShader  = compile PS_PROFILE psCubeSkyboxColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for cubemapped sky box.
*/
technique tSkyboxColor
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile VS_PROFILE vsSkyboxColor();
        PixelShader  = compile PS_PROFILE psSkyboxColor(false);
    }
}

technique tSkyboxColorHDR
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile VS_PROFILE vsSkyboxColor();
        PixelShader  = compile PS_PROFILE psSkyboxColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for particles.
*/
technique tParticleColor
{
    pass p0
    {
        CullMode     = None;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        Sampler[0]   = <DiffSampler>;
        VertexShader = compile VS_PROFILE vsParticleColor();
        PixelShader  = compile PS_PROFILE psParticleColor(false);
    }
}

technique tParticleColorHDR
{
    pass p0
    {
        CullMode     = None;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        Sampler[0]   = <DiffSampler>;
        VertexShader = compile VS_PROFILE vsParticleColor();
        PixelShader  = compile PS_PROFILE psParticleColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for particle2's.
*/
technique tParticle2Color
{
    pass p0
    {
        CullMode     = None;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        Sampler[0]   = <DiffSampler>;
        VertexShader = compile VS_PROFILE vsParticle2Color();
        PixelShader  = compile PS_PROFILE psParticleColor(false);
    }
}

technique tParticle2ColorHDR
{
    pass p0
    {
        CullMode     = None;
        SrcBlend     = <AlphaSrcBlend>;
        DestBlend    = <AlphaDstBlend>;
        Sampler[0]   = <DiffSampler>;
        VertexShader = compile VS_PROFILE vsParticle2Color();
        PixelShader  = compile PS_PROFILE psParticleColor(true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for final frame compose.
*/
technique tCompose
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;
        VertexShader     = compile VS_PROFILE vsCompose();
        PixelShader      = compile PS_PROFILE psCompose();
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "layered"
*/
technique tLayeredDepth
{
    pass p0
    {
        CullMode     = <CullMode>;
        VertexShader = compile VS_PROFILE vsStaticDepth();
        PixelShader  = compile PS_PROFILE psStaticDepth();
    }
}

technique tLayeredColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLayeredColor();
        PixelShader  = compile PS_PROFILE psLayeredColor(false, false);
    }
}

technique tLayeredColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLayeredColor();
        PixelShader  = compile PS_PROFILE psLayeredColor(true, false);
    }
}

technique tLayeredColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLayeredColor();
        PixelShader  = compile PS_PROFILE psLayeredColor(false, true);
    }
}

technique tLayeredColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile VS_PROFILE vsLayeredColor();
        PixelShader  = compile PS_PROFILE psLayeredColor(true, true);
    }
}

//------------------------------------------------------------------------------
/**
    Techniques for shader "multilayered"
*/
technique tMultiLayeredDepth
{
    pass p0
    {
        CullMode     = <CullMode>;
        VertexShader = compile VS_PROFILE vsStaticDepth();
        PixelShader  = compile PS_PROFILE psStaticDepth();
    }
}

technique tMultiLayeredColor
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = True;
        SrcBlend = One;
        DestBlend = SrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;
        VertexShader = compile VS_PROFILE vsMultiLayeredColor();
        PixelShader  = compile PS_PROFILE psMultiLayeredColor(false, false);
    }
}

technique tMultiLayeredColorHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = True;
        SrcBlend = One;
        DestBlend = SrcAlpha;
        AlphaRef         = 1;
        VertexShader = compile VS_PROFILE vsMultiLayeredColor();
        PixelShader  = compile PS_PROFILE psMultiLayeredColor(true, false);
    }
}

technique tMultiLayeredColorShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = True;
        SrcBlend = One;
        DestBlend = SrcAlpha;
        AlphaRef         = 1;
        VertexShader = compile VS_PROFILE vsMultiLayeredColor();
        PixelShader  = compile PS_PROFILE psMultiLayeredColor(false, true);
    }
}

technique tMultiLayeredColorHDRShadow
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = True;
        SrcBlend = One;
        DestBlend = SrcAlpha;
        AlphaRef         = 1;
        VertexShader = compile VS_PROFILE vsMultiLayeredColor();
        PixelShader  = compile PS_PROFILE psMultiLayeredColor(true, true);
    }
}


//------------------------------------------------------------------------------
/**
    Techniques for shader "gui3d"
*/
technique tGui3DColor
{
    pass p0
    {
        CullMode = <CullMode>;
        VertexShader = compile VS_PROFILE vsGui3D();
        PixelShader  = compile PS_PROFILE psGui3D();
    }
}

//------------------------------------------------------------------------------
/**
*/
technique tWaterDepthCompose
{
    pass p0
    {
        ZWriteEnable     = False;
        ZEnable          = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        StencilEnable    = False;

        VertexShader = compile vs_2_0 vsCompose();
        PixelShader = compile ps_2_0 psWaterDepthCompose();
    }
}

//------------------------------------------------------------------------------
//  water.fx
//
//  Water shader from Mathias.
//
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 InvView;

float FresnelBias;
float FresnelPower;

//------------------------------------------------------------------------------
struct vsInputWater
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
};

struct vsOutputWater
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 row0         : TEXCOORD1; // first row of the 3x3 transform from tangent to cube space
    float3 row1         : TEXCOORD2; // second row of the 3x3 transform from tangent to cube space
    float3 row2         : TEXCOORD3; // third row of the 3x3 transform from tangent to cube space
    float4 bumpCoord0   : TEXCOORD4;
    float4 pos          : TEXCOORD5;
    float3 eyeVector    : TEXCOORD6;
};

//------------------------------------------------------------------------------
//  vertex shader
//------------------------------------------------------------------------------
vsOutputWater vsWater(const vsInputWater vsIn)
{
    vsOutputWater vsOut;

    float4 pos = vsIn.position;

    // compute output vertex position
    vsOut.position = mul(pos, ModelViewProjection);

    //position for calculate texture in screenspace (pixelshader)
    vsOut.pos = vsOut.position;

    // pass texture coordinates for fetching the normal map
    vsOut.uv0.xy = vsIn.uv0;

    float modTime = fmod(Time, 100.0f);
    vsOut.bumpCoord0.xy = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy;
    vsOut.bumpCoord0.zw = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy * -1.0f;

    // compute the 3x3 tranform from tangent space to object space
    // first rows are the tangent and binormal scaled by the bump scale
    float3x3 objToTangentSpace;

    objToTangentSpace[0] = BumpScale * vsIn.tangent;
    objToTangentSpace[1] = BumpScale * cross(vsIn.normal, vsIn.tangent);
    objToTangentSpace[2] = vsIn.normal;

    vsOut.row0.xyz = mul(objToTangentSpace, Model[0].xyz);
    vsOut.row1.xyz = mul(objToTangentSpace, Model[1].xyz);
    vsOut.row2.xyz = mul(objToTangentSpace, Model[2].xyz);

    // compute the eye vector (going from shaded point to eye) in cube space
    float4 worldPos = mul(pos, Model);

    vsOut.eyeVector = InvView[3] - worldPos; // view inv. transpose contains eye position in world space in last row

    return vsOut;
}

//------------------------------------------------------------------------------
//  pixel shader ueberwasser
//------------------------------------------------------------------------------
float4 psWater(vsOutputWater psIn, uniform bool hdr, uniform bool refraction) : COLOR
{
    // compute the normals
    // with 2 texture it looks more intressting
    float4 N0 = tex2D(BumpSampler, psIn.bumpCoord0.xy) * 2.0 - 1.0;
    float4 N1 = tex2D(BumpSampler, psIn.bumpCoord0.zw) * 2.0 - 1.0;

    // add both normals
    float3 N = N0.xyz + N1.xyz;

    // bring normals in worldspace
    half3x3 m;
    m[0] = psIn.row0;
    m[1] = psIn.row1;
    m[2] = psIn.row2;
    float3 Nw = mul(m, N.xyz);
    Nw = normalize(Nw);

    // compute screen pos for the reflection refraction texture because both textures are in screencoordinates
    float4 pos = psIn.pos;

    // prepare pos - pos is in model view projection when pos.x and pos.y get divided with pos.w
    // you get a value between -1 and 1 but you need a number between 0 and 1
    // later pos.x and pos.y will divided with pos.w so here we precalculate
    pos.x += psIn.pos.w;
    pos.w  = 2 * psIn.pos.w;

    // compute the distortion and prepare pos for texture lookup
    float3 E = normalize(psIn.eyeVector);

    // angle between eyevector and normal important for fresnel and distortion
    float angle = dot(E, Nw);

    //compute fresnel
    float facing = 1.0 - max(angle, 0);
    float fresnel = FresnelBias + (1.0 - FresnelBias) * pow(facing , FresnelPower);

    // compute distortion - the math is not correct but this is more faster information in my document
    // is computed with angle, because when you look from the side the distortion is stronger then you look from over the water
    // is computed with pos.z, because the distortions are in screencoordinates so the distortions have to get smaller with the distance
    float4 disVecRefract    = float4(Nw.x, Nw.y, Nw.z, 0.0) * (1 - angle) * 0.4 / pos.z * pos.w;

    // refraction rotate the refraction texture with 180 degree
    pos.y  = -psIn.pos.y + psIn.pos.w;

    float refractLookUp;
    float4 refractionComp;
    if(refraction)
    {
        // read the depth from the water with full distortion - this is pasted in the alpha value
        refractLookUp   = tex2Dproj(Diff2Sampler, pos + disVecRefract).a;
        // read the refractioncolor with distortion multiplied with the depth of the water so the distortion is at the strand smaller
        refractionComp  = tex2Dproj(Diff2Sampler, pos + disVecRefract * refractLookUp);
        // apply water color in deep direction
        refractionComp.rgb = lerp(refractionComp.rgb, MatDiffuse.rgb, saturate(refractionComp.a));
    }
    else
    {
        // read the depth from the water WITHOUT distortion - this is pasted in the alpha value
        refractLookUp   = tex2Dproj(Diff2Sampler, pos).a;
        // init the refractioncolor
        refractionComp  = float4(0.0, 0.0, 0.0, refractLookUp);
        // apply water color in deep direction
        refractionComp.rgb = MatDiffuse.rgb;
    }

    // apply water color in shallow direction
    refractionComp.rgb = lerp(refractionComp.rgb, MatSpecular.rgb, facing * refractionComp.a);

    // reflection
    pos.y = psIn.pos.y + psIn.pos.w;

    // same as refraction
    float reflectionLookUp  = tex2Dproj(Diff1Sampler, pos + disVecRefract).a;
    float4 reflectionComp   = tex2Dproj(Diff1Sampler, pos + disVecRefract * reflectionLookUp);

    // compute all together
    float4 colorWater;

    // reflection is multiplied with deep because so the reflection is on the strand less and you get soft edges
    colorWater = (1 - refractionComp.a) * refractionComp + refractionComp.a * reflectionComp * fresnel;
    colorWater.a = refraction ? 1.0f : refractLookUp;

    if (hdr)
    {
        return EncodeHDR(colorWater);
    }
    else
    {
        return colorWater;
    }
}

//------------------------------------------------------------------------------
technique tWater
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = true;
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vsWater();
        PixelShader  = compile ps_2_0 psWater(false, false);
    }
}

technique tWaterHDR
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = true;
        SrcBlend  = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vsWater();
        PixelShader  = compile ps_2_0 psWater(true, false);
    }
}

technique tWaterHDRRefraction
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        VertexShader = compile vs_2_0 vsWater();
        PixelShader  = compile ps_2_0 psWater(true, true);
    }
}
