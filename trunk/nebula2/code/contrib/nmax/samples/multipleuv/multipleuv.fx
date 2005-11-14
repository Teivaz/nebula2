//-------------------------------------------------------------------------------
//  multipleuv.fx
//   
//  An standard example shader, which uses different coordinate sets for diffuse an bump map
//
//  (c) Tobias Lang, 2005, based on shaders.fx by Radon Labs GmbH, 2005
//	Augmented Environments Lab. - Georgia Institute of Technology
//  http://www.cc.gatech.edu/ael
//-------------------------------------------------------------------------------

#define VS_PROFILE vs_3_0
#define PS_PROFILE ps_3_0

#define DECLARE_SCREENPOS(X) float4 screenPos : X;
#define PS_SCREENPOS psComputeScreenCoord(psIn.screenPos)
#define VS_SETSCREENPOS(X) vsOut.screenPos = vsComputeScreenCoord(X);


shared float4x4 Model;                  // the model matrix
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix
shared float4x4 ModelView;              // the model * view matrix
shared float4x4 ModelViewProjection;    // the model * view * projection matrix
shared float4x4 InvModel;               // the inverse model matrix
shared float4x4 InvView;                // the inverse view matrix
shared float4x4 InvModelView;           // the inverse model * view matrix

shared float3   LightPos;               // light position in world space
shared int      LightType;              // light type
shared float    LightRange;             // light range
shared float4   LightDiffuse;           // light diffuse component
shared float4   LightSpecular;          // light specular component
shared float4   LightAmbient;           // light ambient component
shared float4   ShadowIndex;            // shadow light index
float3			ModelLightPos;

shared float3   ModelEyePos;            // the eye pos in model space
shared float3   EyePos;                 // the eye pos in world space
shared float4x4 TextureTransform0;      // texture transform for uv set 0
shared float4x4 TextureTransform1;      // texture transform for uv set 1
shared float4x4 TextureTransform2;      // texture transform for uv set 0
shared float4x4 TextureTransform3;      // texture transform for uv set 0

shared float    Time;                      // the current global time
shared float4   DisplayResolution;        // the current display resolution
shared float4   HalfPixelSize;            // half size of a display pixel
bool            AlphaBlendEnable = false;



float BumpScale = 0.0f;

float4 Color0;
float4 MatDiffuse;
float4 MatEmissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
float  MatEmissiveIntensity = 0.0f;
float4 MatSpecular;
float4 MatAmbient;
float  MatSpecularPower;

texture DiffMap0;
texture BumpMap0;


struct vsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;     // texture coordinate uv0, used for diffuse map
    float2 uv1      : TEXCOORD1;     // texture coordinate uv1, used for bump map
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
};

struct vsOutput
{
    float4 position      : POSITION;  // transformed position if the vertex
    float2 uv0           : TEXCOORD0; // texture coordinate uv0, used for diffuse map
    float2 uv1           : TEXCOORD1; // texture coordinate uv1, used for bump map
    float3 lightVec      : TEXCOORD2;
    float3 modelLightVec : TEXCOORD3;
    float3 halfVec       : TEXCOORD4;
    float3 eyePos        : TEXCOORD5;
    DECLARE_SCREENPOS(TEXCOORD6)
};


struct vsInputDepth
{
    float4 position : POSITION;
};

struct vsOutputDepth
{
    float4 position : POSITION;
    float  depth    : TEXCOORD0;
};

//------------------------------------------------------------------------------
/**
    sampler definitions
*/
sampler DiffuseSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
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


//------------------------------------------------------------------------------
/**
    Vertex shader part to compute texture coordinate shadow texture lookups.
*/
float4 vsComputeScreenCoord(float4 pos) {
    return pos;
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
    Compute per-pixel lighting.
    
    NOTE: lightVec.w contains the distance to the light source
*/
float4 psLight(in float4 mapColor, in float3 tangentSurfaceNormal, in float3 lightVec, in float3 modelLightVec, in float3 halfVec)
{
   
    float4 color = mapColor * float4(LightAmbient.rgb + MatEmissive.rgb * MatEmissiveIntensity, MatDiffuse.a);
    // light intensities
    half specIntensity = pow(saturate(dot(tangentSurfaceNormal, normalize(halfVec))), MatSpecularPower); // Specular-Modulation * mapColor.a;
    half diffIntensity = dot(tangentSurfaceNormal, normalize(lightVec));

    float3 diffColor = mapColor.rgb * LightDiffuse.rgb * MatDiffuse.rgb;
    float3 specColor = specIntensity * LightSpecular.rgb * MatSpecular.rgb;

    // attenuation
    if (LightType == 0)
    {
        // point light source
        diffIntensity *=  (1.0f - saturate(length(modelLightVec) / LightRange));
    }
    else
    {
        #if DIRLIGHTS_ENABLEOPPOSITECOLOR
        color.rgb += saturate(-diffIntensity) * DIRLIGHTS_OPPOSITECOLOR * mapColor.rgb;
        #endif    
    }    
    color.rgb += saturate(diffIntensity) * (diffColor.rgb + specColor.rgb);
    return color;
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


//------------------------------------------------------------------------------
/**
    Vertex shader: generate color values for static geometry.
*/

vsOutput vsMultipleUV(const vsInput vsIn) {
  vsOutput vsOut;
  vsOut.position = mul(vsIn.position, ModelViewProjection);
  VS_SETSCREENPOS(vsOut.position);
  vsOut.uv0      = vsIn.uv0;
  vsOut.uv1      = vsIn.uv1;
  vsLight(vsIn.position, vsIn.normal, vsIn.tangent, vsIn.binormal, ModelEyePos, ModelLightPos, vsOut.lightVec, vsOut.modelLightVec, vsOut.halfVec, vsOut.eyePos);
  return vsOut;
}

float4 psMultipleUV( const vsOutput psIn ) : COLOR  {
  half2 uvOffset = half2(0.0f, 0.0f);
  if (BumpScale != 0.0f) 
    uvOffset = ParallaxUv(psIn.uv1, BumpSampler, psIn.eyePos);
  
  float4 diffColor = tex2D(DiffuseSampler, psIn.uv0 + uvOffset);
  float3 tangentSurfaceNormal = (tex2D(BumpSampler, psIn.uv1 + uvOffset).rgb * 2.0f) - 1.0f;    
  float4 baseColor = psLight(diffColor, tangentSurfaceNormal, psIn.lightVec, psIn.modelLightVec, psIn.halfVec);
  
  return baseColor;
}

vsOutputDepth vsDepth(float4 position : POSITION)
{
    vsOutputDepth vsOut;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.depth = vsOut.position.z;
    return vsOut;
}

float4 psDepth(const vsOutputDepth psIn) : COLOR {
  return float4(psIn.depth, 0.0f, 0.0f, 1.0f);
}

technique tDepth
{
    pass p0
    {
       VertexShader = compile VS_PROFILE vsDepth();
       PixelShader  = compile PS_PROFILE psDepth();
    }
}

technique tmultipleuv
{
    pass p0
    {
       CullMode = NONE;
       VertexShader = compile VS_PROFILE vsMultipleUV();
       PixelShader  = compile PS_PROFILE psMultipleUV();
    }
}