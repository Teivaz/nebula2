//------------------------------------------------------------------------------
//  shaders.fx
//  Standard shaders for the DX7 render path.
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    shader parameters
*/
shared float4x4 Model;
shared float4x4 InvModelView;               // the inverse model*view matrix
shared float3 ModelEyePos;
shared float4x4 ModelViewProjection;
shared float4x4 TextureTransform0;
shared float4x4 TextureTransform1;
shared float    Time;

float3 ModelLightPos;
shared float3   LightPos;
shared int      LightType;
shared float    LightRange;
shared float4   LightDiffuse;
shared float4   LightSpecular;
shared float4   LightAmbient;
shared float4   ShadowIndex;

float4 MatDiffuse;
float4 MatSpecular;
float4 MatAmbient;
float4 MatEmissive;
float  MatTranslucency;
float  MatSpecularPower;

bool AlphaBlendEnable;
int AlphaRef;
int CullMode = 2;
int AlphaSrcBlend = 5;
int AlphaDstBlend = 6;
int VertexStreams;
float4 VertexWeights1;
float4 VertexWeights2;
matrix<float,4,3> JointPalette[72];     // the joint palette for skinning
float4 Wind;                        // wind direction, strength in .w
float3x3 Swing;                     // the swing rotation matrix
float3 BoxMinPos;                   // model space bounding box min
float3 BoxMaxPos;                   // model space bounding box max
float3 BoxCenter;                   // model space bounding box center
float SpriteSize;                   // the leaf size
float SpriteSwingTime;              // swing period
float SpriteSwingAngle;             // swing angle modulator
float InnerLightIntensity;          // light intensity at center
float OuterLightIntensity;          // light intensity at periphery
float2 Velocity = float2(0.0, 0.0); // UVAnimaton2D

static const float4 GenAngle = { 3.54824948311, -11.6819286346, 10.4263944626, -2.29271507263 };

static const float4x4 Ident = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f, 
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };
                                
// detail texture scale for the layered shader, zoom by 40 as default
static const float4x4 DetailTexture = {40.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f,40.0f, 0.0f, 0.0f, 
                                        0.0f, 0.0f,40.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f,40.0f };
                                        
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

float4x4 UVStretch0 =    {1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f, 
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f };                                
float4x4 UVStretch1 =    {1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f, 
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f };                                
float4x4 UVStretch2 =    {1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f, 
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f };                                
float4x4 UVStretch3 =    {1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f, 
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f };                                
float4x4 UVStretch4 =    {1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f, 
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f };                                
float4x4 UVStretch5 =    {1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f, 
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f };                                

texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
texture DiffMap3;
texture CubeMap0;
texture AmbientMap0;
texture AmbientMap1;
texture AmbientMap2;
texture AmbientMap3;
texture BumpMap0;
texture BumpMap1;
texture BumpMap2;
texture BumpMap3;
texture SpecMap0;
texture SpecMap1;
texture SpecMap2;
texture SpecMap3;
texture NoiseMap0;
texture NoiseMap1;
texture NoiseMap2;
texture NoiseMap3;

#include "shaders:../lib/lib.fx"

//------------------------------------------------------------------------------
/**
    In/Out-Definitions for emulated vertex shaders.
*/
struct vsInputBlended
{
    float4 position0 : POSITION0;
    float4 position1 : POSITION1;
    float4 position2 : POSITION2;
    float4 position3 : POSITION3;
    float4 position4 : POSITION4;
    float4 position5 : POSITION5;
    float4 position6 : POSITION6;
    float3 normal0   : NORMAL0;
    float3 normal1   : NORMAL1;
    float3 normal2   : NORMAL2;
    float3 normal3   : NORMAL3;
    float3 normal4   : NORMAL4;
    float3 normal5   : NORMAL5;
    float3 normal6   : NORMAL6;
    float2 uv0      : TEXCOORD0;
};

struct vsInputSkinned
{
    float4 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES; 
};

struct vsInputSkinnedML
{
    float4 position : POSITION;
    float3 normal 	: NORMAL;  
    float2 uv0 :      TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
    float4 mask     : COLOR0; 
};

struct vsInputLeaf
{
    float4 position : POSITION;
    float3 extrude  : NORMAL;           // sprite extrusion vector
    float2 uv0 :      TEXCOORD0;
};

struct vsInputTree
{
    float3 position : POSITION;
    float3 normal :   NORMAL;  
    float2 uv0 :      TEXCOORD0;
};

struct vsInputParticle
{
    float4 position  : POSITION; 
    float2 uv0       : TEXCOORD0;
    float2 extrude   : TEXCOORD1;
    float2 transform : TEXCOORD2;
    float4 color     : COLOR0;   
};

struct vsOutputFFP
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

struct vsOutputVertexColorML
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 cubeVec  : TEXCOORD1;
    float4 diffuse  : COLOR0;
};

struct vsInputParticle2
{
    float4 position  : POSITION;    // the particle position in world space
    float3 velocity  : NORMAL;      // the particle coded uv and corners,rotation and scale
    float2 uv        : TEXCOORD0;   // the particle coded uv and corners,rotation and scale
    float4 data      : COLOR0;      // the particle coded uv and corners,rotation and scale
};

struct vsOutputParticle2
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

struct vsInputStaticColor
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : NORMAL;
};

//------------------------------------------------------------------------------
/**
    Sampler definitions
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
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler Diff3Sampler  = sampler_state
{
    Texture = <DiffMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler LightmapSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
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

sampler WeightSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler DetailSampler  = sampler_state
{
    Texture = <AmbientMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Point;
    MipMapLodBias = -0.75;
};

sampler LayerSampler0 = sampler_state
{
    Texture = <NoiseMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler LayerSampler1 = sampler_state
{
    Texture = <NoiseMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler LayerSampler2 = sampler_state
{
    Texture = <NoiseMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    MipMapLodBias = -0.75;
};

sampler MultiLayerSampler0 = sampler_state
{
    Texture = <NoiseMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerSampler1 = sampler_state
{
    Texture = <BumpMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerSampler2 = sampler_state
{
    Texture = <AmbientMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerSampler3 = sampler_state
{
    Texture = <AmbientMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerSampler4 = sampler_state
{
    Texture = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerTextureSampler0 = sampler_state
{
    Texture = <SpecMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerTextureSampler1 = sampler_state
{
    Texture = <SpecMap1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerTextureSampler2 = sampler_state
{
    Texture = <SpecMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerTextureSampler3 = sampler_state
{
    Texture = <SpecMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerTextureSampler4 = sampler_state
{
    Texture = <BumpMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};
sampler MultiLayerTextureSampler5 = sampler_state
{
    Texture = <BumpMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};


//------------------------------------------------------------------------------
/**
    Compute lighting for rendering through vertex shaders.
*/
float4 vsLight(float3 pos, float3 normal)
{
    float3 L;
    if (LightType == 0)
    {
        // point lights
        L = normalize(ModelLightPos - pos);
    }
    else
    {
        // directional lights
        L = normalize(ModelLightPos);
    }
    float4 light = float4(MatEmissive.rgb + LightAmbient.rgb + MatDiffuse.rgb * LightDiffuse.rgb * max(0, dot(normal, L)), MatDiffuse.a);
    return light;
}

//------------------------------------------------------------------------------
/**
    Compute specular lighting for rendering through vertex shaders.
*/
float4 vsSpecularLight(float3 pos, float3 normal, float specPow, float4 specCol)
{
	float3 lVec;
    if (LightType == 0)
    {
        // point lights
        lVec = normalize(ModelLightPos - pos);
    }
    else
    {
        // directional lights
        lVec = normalize(ModelLightPos);
    }
    float3 eVec = normalize(ModelEyePos - pos);
    float3 hVec = normalize(lVec + eVec);
    
	float primSpecIntensity = pow(saturate(dot(normal, hVec)), specPow);
	float primDiffIntensity = max(0, dot(normal, lVec));
	
	float4 specColor = primSpecIntensity * LightSpecular[0] * specCol * primDiffIntensity;
	float4 light = float4(specColor.rgb + MatEmissive.rgb + LightAmbient.rgb + MatDiffuse.rgb * LightDiffuse.rgb * primDiffIntensity, MatDiffuse.a);
	
	return light;
}

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for blended geometry.
*/
vsOutputFFP vsBlended(const vsInputBlended vsIn)
{
    vsOutputFFP vsOut;
    float4 pos = 0;
    float3 normal = 0;
    if (VertexStreams > 0)
    {
        pos = vsIn.position0;
        normal = vsIn.normal0;
        if (VertexStreams > 1)
        {
            pos += (vsIn.position1-vsIn.position0) * VertexWeights1.y;
            normal += (vsIn.normal1-vsIn.normal0) * VertexWeights1.y;
            if (VertexStreams > 2)
            {
                pos += (vsIn.position2-vsIn.position0) * VertexWeights1.z;
                normal += (vsIn.normal2-vsIn.normal0) * VertexWeights1.z;                
                if (VertexStreams > 3)
                {
                    pos += (vsIn.position3-vsIn.position0) * VertexWeights1.w;
                    normal += (vsIn.normal3-vsIn.normal0) * VertexWeights1.w;
                    if (VertexStreams > 4)
                    {
                        pos += (vsIn.position4-vsIn.position0) * VertexWeights2.x;
                        normal += (vsIn.normal4-vsIn.normal0) * VertexWeights2.x;
                        if (VertexStreams > 5)
                        {
                            pos += (vsIn.position5-vsIn.position0) * VertexWeights2.y;
                            normal += (vsIn.normal5-vsIn.normal0) * VertexWeights2.y;
                            if (VertexStreams > 6)
                            {
                                pos += (vsIn.position6-vsIn.position0) * VertexWeights2.z;
                                normal += (vsIn.normal6-vsIn.normal0) * VertexWeights2.z;
                            }
                        }
                    }
                }                
            }
        }
    }

    vsOut.position = mul(pos, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = vsLight(pos, normal);

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for skinned geometry.
*/
vsOutputFFP vsSkinned(const vsInputSkinned vsIn)
{
    vsOutputFFP vsOut;

    // get skinned position, normal and tangent    
    float3 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
    
    // transform vertex position
    vsOut.position = transformStatic(skinPos, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = vsLight(skinPos, skinNormal);

    return vsOut;
}

// //------------------------------------------------------------------------------
// /**
//     Emulated vertex shader for skinned geometry with specular lighting and
//     vertexColor driven Multilayertexturing
// */
// vsOutputVertexColorML vsSkinnedML(const vsInputSkinnedML vsIn, uniform int layer)
// {
//     vsOutputVertexColorML vsOut;

//     // get skinned position, normal and tangent    
//     float3 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
//     float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
//     
//     // transform vertex position
//     vsOut.position = transformStatic(skinPos, ModelViewProjection);
//     //vsOut.diffuse = vsSpecularLight(skinPos, skinNormal, MatSpecularPower, MatSpecular);
//     vsOut.diffuse = vsSpecularLight(skinPos, skinNormal, 16, float4(1,1,1,1));
//         
//     if (layer == 0) {vsOut.diffuse.a *= 1;}
//     else if (layer == 1) {vsOut.diffuse.a *= vsIn.mask.r;}
//     else if (layer == 2) {vsOut.diffuse.a *= vsIn.mask.g;}
//     else if (layer == 3) {vsOut.diffuse.a *= vsIn.mask.b;}
//     else if (layer == 4) {vsOut.diffuse.a *= vsIn.mask.a;}
//     //else if (layer == 4) {vsOut.diffuse = 1;}
//     
//     vsOut.uv0 = vsIn.uv0;
//     
//     return vsOut;
// }

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for skinned geometry with specular lighting and
    vertexColor driven Multilayertexturing
*/
vsOutputVertexColorML vsSkinnedML(const vsInputSkinnedML vsIn, uniform int layer)
{
    vsOutputVertexColorML vsOut;

    // get skinned position, normal and tangent    
    float3 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    float3 skinNormal  = skinnedNormal(vsIn.normal, vsIn.weights, vsIn.indices, JointPalette);
    
    // transform vertex position
    vsOut.position = transformStatic(skinPos, ModelViewProjection);
    vsOut.diffuse = vsSpecularLight(skinPos, skinNormal, MatSpecularPower, MatSpecular);
    float3 viewVec = skinPos - ModelEyePos;
    vsOut.cubeVec = reflect(skinNormal, viewVec);
        
    if (layer == 0) {}
    else if (layer == 1) {vsOut.diffuse.a *= vsIn.mask.r;}
    else if (layer == 2) {vsOut.diffuse.a *= vsIn.mask.g;}
    else if (layer == 3) {vsOut.diffuse.a *= vsIn.mask.b;}
    else if (layer == 4) {vsOut.diffuse.a *= vsIn.mask.a;}
    
    vsOut.uv0 = vsIn.uv0;
    
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for leafs.
*/
vsOutputFFP vsLeaf(const vsInputLeaf vsIn)
{
    vsOutputFFP vsOut;

    // compute swayed position in model space (see tree.fx for details)
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);
    float4 rotPosition  = float4(mul(Swing, vsIn.position), 1.0f);
    float4 position = lerp(vsIn.position, rotPosition, ipol);
        
    // compute rotation angle
    float t = frac((Time / SpriteSwingTime) + vsIn.position.x);
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
    float3 extrudeVec = mul(rot, vsIn.extrude);
    float3 modelExtrudeVec = mul(extrudeVec, (float3x3)InvModelView);
        
    // extrude to corner position
    position.xyz += modelExtrudeVec.xyz;
    
    // transform to projection space
    vsOut.position = mul(position, ModelViewProjection);
    
    // compute light vec and half vec for pixel shader lighting,
    // note: the surface coordinate system is identical with
    // the view coordinate system, so that's easy
    float3 lVec = normalize(ModelLightPos - position);
    float3 eVec = normalize(ModelEyePos - position);
    
    // compute a translucency/reflection factor (when eye looks towards sun)
    float3 eyeLightVector = normalize(ModelLightPos - ModelEyePos);
    float3 eyePosVector   = normalize(position - ModelEyePos);
    float translucency = abs(dot(eyeLightVector, eyePosVector) * MatTranslucency);
    
    // compute a fake lighting intensity (dot(N, L)
    float3 posCenter = vsIn.position - BoxCenter;
//    float diffIntensity = 1.0 + clamp(dot(normalize(posCenter), lVec), -1.0, 0.0); // max for angles < 90 degrees
    float diffIntensity = (dot(normalize(posCenter), lVec) + 1.0)*0.5;
  
    // compute a selfshadowing value
    float relDistToCenter = length(posCenter) / distance(BoxMaxPos, BoxCenter);
    relDistToCenter *= relDistToCenter;
    float selfShadow = lerp(InnerLightIntensity, OuterLightIntensity, relDistToCenter);
    
    float intensity = diffIntensity * selfShadow;
    
    vsOut.diffuse = LightDiffuse * MatDiffuse * intensity + LightAmbient;
    vsOut.uv0 = vsIn.uv0;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for trees.
*/
vsOutputFFP vsTree(const vsInputTree vsIn)
{
    vsOutputFFP vsOut;

    // compute lerp factor based on height above min y
    float ipol = (vsIn.position.y - BoxMinPos.y) / (BoxMaxPos.y - BoxMinPos.y);

    // compute rotated vertex position, normal in model space
    float3 rotPosition  = mul(Swing, vsIn.position);
    float3 rotNormal    = mul(Swing, vsIn.normal);
    
    // lerp between original and rotated pos
    float3 lerpPosition = lerp(vsIn.position, rotPosition, ipol);
    float3 lerpNormal   = lerp(vsIn.normal, rotNormal, ipol);
    
    // transform vertex position
    vsOut.position = transformStatic(lerpPosition, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.diffuse = vsLight(lerpPosition, lerpNormal);

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Emulated vertex shader for particles.
*/
vsOutputFFP vsParticle(const vsInputParticle vsIn)
{   
    vsOutputFFP vsOut;

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
    extrude        *= size;
    extrude         =  mul(rot, extrude);    
    extrude         =  mul(extrude, (float3x3) InvModelView);
    position.xyz   += extrude.xyz;
            
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0      = vsIn.uv0;
    vsOut.diffuse  = vsIn.color;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Emulated Vertex shader for particle2's.
*/
vsOutputParticle2 vsParticle2(const vsInputParticle2 vsIn)
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

    vsOutputParticle2 vsOut;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0      = vsIn.uv;
    vsOut.diffuse  = rgba;

    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader: process vertices for 3d gui. This will be a orthogonal
    projection.
*/
vsOutputFFP vsGui3D(const vsInputStaticColor vsIn)
{
    vsOutputFFP vsOut;
    vsOut.position = mul(mul(vsIn.position, Model), OrthoProjection);
    vsOut.uv0 = vsIn.uv0;
    //vsOut.diffuse = vsLight(vsIn.position, vsIn.normal);
    vsOut.diffuse = MatDiffuse;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
    Vertex shader for UV Animation
*/
vsOutputFFP vsUvAnimation(const vsInputStaticColor vsIn)
{
    vsOutputFFP vsOut;
    float4 pos = transformStatic(vsIn.position, ModelViewProjection);
    vsOut.position = pos;
    vsOut.diffuse = vsLight(pos, vsIn.normal);
    
    // animate uv
    vsOut.uv0 = vsIn.uv0 + (Velocity * Time);
    
    return vsOut;
}


//------------------------------------------------------------------------------
/**
    Techniques.
*/
technique tEmpty
{
    pass p0
    {
        // empty
    }
}

technique tStatic
{
    pass p0
    {        
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransformFlags[0] = Count2;
        
        CullMode            = <CullMode>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX1;

        TexCoordIndex[0] = 0;

        Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;

        ColorOp[1]   = Disable;
    }
}

technique tStaticATest
{
    pass p0
    {        
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransformFlags[0] = Count2;
        
        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;        
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX1;

        TexCoordIndex[0] = 0;

        Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;

        ColorOp[1]   = Disable;
    }
}

technique tLightmapped
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <TextureTransform1>;
        
        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
 
        Lighting = false;
        
        FVF = XYZ | NORMAL | TEX2;
        
        TexCoordIndex[0] = 0;
        //TextureTransformFlags[0] = Count2;

        TexCoordIndex[1] = 1;
        //TextureTransformFlags[1] = Count2;
                
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <LightmapSampler>;
        
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = 0;
        
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        ColorOp[2]   = Disable;
    }
}

technique tLightmapped2
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <TextureTransform1>;
        
        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                
 
        Lighting = true;
        
        FVF = XYZ | NORMAL | TEX2;
        
        TexCoordIndex[0] = 1;
        //TextureTransformFlags[0] = Count2;

        TexCoordIndex[1] = 0;
        //TextureTransformFlags[1] = Count2;
                
        Sampler[0] = <LightmapSampler>;
        Sampler[1] = <DiffSampler>;
        
        // ADDSIGNED (Arg1 + Arg2 - 0.5) - shadow side becames black if lightmap is darker then 0.5
        // ADDSMOOTH (Arg1 + Arg2 (1 - Arg1)) - shadow side never becomes black
        
        ColorOp[0]   = AddSigned; 
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        ColorOp[2]   = Disable;
    }
}

technique tUnlightedLightmapped2
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <TextureTransform1>;
        
        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;
        MaterialAmbient     = {0.0, 0.0, 0.0, 1.0};                
 
        Lighting = false;
        
        FVF = XYZ | NORMAL | TEX2;
        
        TexCoordIndex[0] = 1;
        //TextureTransformFlags[0] = Count2;

        TexCoordIndex[1] = 0;
        //TextureTransformFlags[1] = Count2;
                
        Sampler[0] = <LightmapSampler>;
        Sampler[1] = <DiffSampler>;
        
        // ADDSIGNED (Arg1 + Arg2 - 0.5) - shadow side becames black if lightmap is darker then 0.5
        // ADDSMOOTH (Arg1 + Arg2 (1 - Arg1)) - shadow side never becomes black
        
        ColorOp[0]   = SelectArg1; 
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        ColorOp[2]   = Disable;
    }
}

technique tUnlightedAlphaLightmapped2
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <TextureTransform1>;
        
        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>; 
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;
        MaterialAmbient     = {0.0, 0.0, 0.0, 1.0};                
 
        Lighting = false;
        
        FVF = XYZ | NORMAL | TEX2;
        
        TexCoordIndex[0] = 1;
        //TextureTransformFlags[0] = Count2;

        TexCoordIndex[1] = 0;
        //TextureTransformFlags[1] = Count2;
                
        Sampler[0] = <LightmapSampler>;
        Sampler[1] = <DiffSampler>;
        
        // ADDSIGNED (Arg1 + Arg2 - 0.5) - shadow side becames black if lightmap is darker then 0.5
        // ADDSMOOTH (Arg1 + Arg2 (1 - Arg1)) - shadow side never becomes black
        
        ColorOp[0]   = SelectArg1; 
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Diffuse;
        AlphaArg2[0] = Diffuse;
        
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2]   = Disable;
        AlphaOp[2]   = Disable;
    }
}

technique tEnvironment
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <Ident>;

        AlphaRef            = <AlphaRef>;
        CullMode            = <CullMode>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;        
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        //TextureTransformFlags[0] = Count2;
                
        TexCoordIndex[1] = CameraSpaceReflectionVector;
        //TextureTransformFlags[1] = Count3;
        
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <EnvironmentSampler>;

        // Base Texture
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;        

        // Reflection Texture
        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = SelectArg1;
        AlphaArg1[1] = Current;

        // Lighting
        ColorOp[2]   = Modulate;
        ColorArg1[2] = Diffuse;
        ColorArg2[2] = Current;
        AlphaOp[2]   = SelectArg1;
        AlphaArg1[2] = Current;

        ColorOp[3]   = Disable;
        AlphaOp[3]   = Disable;
    }
}

technique tStaticAlpha
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;

        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;        
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;        
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX1;

        TexCoordIndex[0] = 0;
        //TextureTransformFlags[0] = Count2;
   
        Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;
    }
}

technique tSkinnedAlpha
{
    pass p0
    {
        CullMode  = <CullMode>;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;        
        
        VertexShader = compile vs_2_0 vsSkinned();
        
        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

technique tStaticAlpha2
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <TextureTransform1>;

        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;        
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;
        //TextureTransformFlags[0] = Count2;
        //TextureTransformFlags[1] = Count2;
   
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <Diff1Sampler>;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;

        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = BlendCurrentAlpha;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;
                
        ColorOp[2]   = Disable;
        AlphaOp[2]   = Disable;
    }
}

technique tEnvironmentAlpha
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;
        //TextureTransform[1] = <Ident>;

        CullMode            = <CullMode>;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;        
        MaterialDiffuse     = <MatDiffuse>;
        MaterialEmissive    = <MatEmissive>;        
        MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        //TextureTransformFlags[0] = Count2;
                
        TexCoordIndex[1] = CameraSpaceReflectionVector;
        //TextureTransformFlags[1] = Count3;
        
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <EnvironmentSampler>;

        // Base Texture
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;        
        
        // Reflection Texture
        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = SelectArg1;
        AlphaArg1[1] = Current;
       
        // Lighting
        ColorOp[2]   = Modulate;
        ColorArg1[2] = Diffuse;
        ColorArg2[2] = Current;
        AlphaOp[2]   = SelectArg1;
        AlphaArg1[2] = Current;

        ColorOp[3]   = Disable;
        AlphaOp[3]   = Disable;
    }
}

technique tSkybox
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        //TextureTransform[0] = <TextureTransform0>;

        AlphaRef            = 0;
        CullMode            = NONE;
        MaterialAmbient     = {0.0f, 0.0f, 0.0f, 0.0f };
        MaterialDiffuse     = {0.0f, 0.0f, 0.0f, 0.0f };
        MaterialEmissive    = <MatDiffuse>;

        LightEnable[0] = True;  

        FVF = XYZ | NORMAL | TEX1;
        
        TexCoordIndex[0] = 0;
        //TextureTransformFlags[0] = Count2;
   
        Sampler[0] = <DiffSampler>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

technique tBlended
{
    pass p0
    {
        AlphaRef     = <AlphaRef>;
        CullMode     = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        
        VertexShader = compile vs_2_0 vsBlended();

        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

technique tSkinned
{
    pass p0
    {
        AlphaRef = <AlphaRef>;
        CullMode = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        
        VertexShader = compile vs_2_0 vsSkinned();
        
        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

technique tLeaf
{
    pass p0
    {
        CullMode = <CullMode>;
        AlphaRef = <AlphaRef>;
        AlphaBlendEnable = <AlphaBlendEnable>;

        VertexShader = compile vs_2_0 vsLeaf();

        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

technique tTree
{
    pass p0
    {
        AlphaRef     = <AlphaRef>;
        CullMode     = <CullMode>;
        AlphaBlendEnable = <AlphaBlendEnable>;
        
        VertexShader = compile vs_2_0 vsTree();
                
        Sampler[0]   = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
    }
}

technique tTerrain
{
    pass p0
    {
        WorldTransform[0] = <Model>;

        CullMode = <CullMode>;                   
        FVF = XYZ | TEX2;
       
        MaterialDiffuse = {1.0, 1.0, 1.0, 1.0};     
        MaterialEmissive    = <MatEmissive>;        

        Sampler[0]      = <WeightSampler>;
        Sampler[1]      = <DetailSampler>;
   
        TexCoordIndex[0]         = CAMERASPACEPOSITION;
        //TextureTransformFlags[0] = Count3;
        //TextureTransform[0]      = <TextureTransform0>;

        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;

        TexCoordIndex[1]         = CAMERASPACEPOSITION;
        //TextureTransformFlags[1] = Count3;
        //TextureTransform[1]      = <TextureTransform1>;

        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        
        ColorOp[2] = Disable;
        AlphaOp[0] = Disable; 
    }
}

technique tParticle
{
    pass p0
    {
        //TextureTransformFlags[0] = 0;
        CullMode  = None;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;        

        VertexShader = compile vs_2_0 vsParticle();

        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
           
        ColorOp[1] = Disable;        
        AlphaOp[1] = Disable;
    }
}

technique tParticle2
{
    pass p0
    {
        //TextureTransformFlags[0] = 0;
        CullMode  = None;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;        

        VertexShader = compile vs_2_0 vsParticle2();

        Sampler[0] = <DiffSampler>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
           
        ColorOp[1] = Disable;        
        AlphaOp[1] = Disable;
    }
}

technique tLayered
{
    pass p0
    {        
        WorldTransform[0] = <Model>;
        CullMode          = <CullMode>;
        AlphaTestEnable   = False;
        MaterialDiffuse   = <MatDiffuse>;
        MaterialEmissive  = <MatEmissive>;        
        MaterialAmbient   = {1.0, 1.0, 1.0, 1.0};                

        FVF = XYZ | NORMAL | TEX2;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <DetailTexture>;
        Sampler[0]               = <DiffSampler>;
               
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;

        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable; 
    }
    pass p1
    {
        WorldTransform[0] = <Model>;
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = True;
        AlphaRef         = 1;

        Sampler[0]       = <Diff1Sampler>;
        Sampler[1]       = <LayerSampler0>;
        
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <Ident>;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Current;
        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
       
        ColorOp[1]   = SelectArg1;
        ColorArg1[1] = Current;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;

        ColorOp[2] = Disable;
        AlphaOp[2] = Disable; 
    }
    pass p2
    {
        WorldTransform[0] = <Model>;
        Sampler[0]   = <Diff2Sampler>;
        Sampler[1]   = <LayerSampler1>;
 
    }
    pass p3
    {
        WorldTransform[0] = <Model>;
        Sampler[0]   = <Diff3Sampler>;
        Sampler[1]   = <LayerSampler2>; 
    }       
        
}


technique tMultiLayered
{
    pass p0
    {        
        WorldTransform[0] = <Model>;
        CullMode          = <CullMode>;
        MaterialDiffuse   = <MatDiffuse>;
        MaterialEmissive  = <MatEmissive>;
         
        MaterialAmbient   = {1.0, 1.0, 1.0, 1.0};                
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;
        AlphaRef         = 1;

        FVF = XYZ | NORMAL | TEX2;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <Ident>;
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <UVStretch0>;
        
        Sampler[0]               = <MultiLayerSampler0>;
        Sampler[1]               = <MultiLayerTextureSampler0>;

        // Load Transparency Map, includes lightmap
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Current;
        ColorArg2[0] = Texture;
        AlphaOp[0]   = SelectArg2;
        AlphaArg1[0] = Current;
        AlphaArg2[0] = Texture;

        // Load Texture Map
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Current;
        AlphaArg2[1] = Texture;

        ColorOp[2]   = Disable;
        AlphaOp[2]   = Disable;

    }
    
    pass p1
    {      
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch1>;
        Sampler[0]               = <MultiLayerSampler1>;
        Sampler[1]               = <MultiLayerTextureSampler1>;
    }  
    
    pass p2
    {      
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch2>;
        Sampler[0]               = <MultiLayerSampler2>;
        Sampler[1]               = <MultiLayerTextureSampler2>;
    }  
    pass p3
    {      
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch3>;
        Sampler[0]               = <MultiLayerSampler3>;
        Sampler[1]               = <MultiLayerTextureSampler3>;
    }  
    pass p4
    {      
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch4>;
        Sampler[0]               = <MultiLayerSampler4>;
        Sampler[1]               = <MultiLayerTextureSampler4>;
    }  
    
};      

technique tSkinnedML
{
    pass p0
    {        
        WorldTransform[0] = <Model>;
        CullMode          = <CullMode>;
        MaterialDiffuse   = <MatDiffuse>;
        MaterialEmissive  = <MatEmissive>;
         
        MaterialAmbient   = {1.0, 1.0, 1.0, 1.0};                
        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;
        AlphaRef         = 1;

        FVF = XYZ | NORMAL | TEX2;

        TexCoordIndex[0]         = 0;
        TextureTransformFlags[0] = Count2;
        TextureTransform[0]      = <Ident>;
        TexCoordIndex[1]         = 0;
        TextureTransformFlags[1] = Count2;
        TextureTransform[1]      = <UVStretch0>;
        
        VertexShader = compile vs_2_0 vsSkinnedML(0);

        Sampler[1] = <DiffSampler>;
        Sampler[0] = <DiffSampler>;       
        
        // Load Transparency Map, includes lightmap
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Current;
        ColorArg2[0] = Texture;
        AlphaOp[0]   = SelectArg2;
        AlphaArg1[0] = Current;
        AlphaArg2[0] = Diffuse;

        // Load Texture Map
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = Modulate;
        AlphaArg1[1] = Current;
        AlphaArg2[1] = Texture;

        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;

    }
    
    pass p1
    {      
	    VertexShader = compile vs_2_0 vsSkinnedML(1);
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch1>;
        Sampler[0]               = <Diff1Sampler>;
        Sampler[1]               = <Diff1Sampler>;
    }  
    
    pass p2
    {      
	    VertexShader = compile vs_2_0 vsSkinnedML(2);
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch2>;
        Sampler[0]               = <Diff2Sampler>;
        Sampler[1]               = <Diff2Sampler>;
    }  
    pass p3
    {   
	    VertexShader = compile vs_2_0 vsSkinnedML(3);
        TextureTransform[0]      = <Ident>;
        TextureTransform[1]      = <UVStretch3>;
        Sampler[0]               = <Diff3Sampler>;
        Sampler[1]               = <Diff3Sampler>;
    } 
    
    pass p4
    {   
        VertexShader = compile vs_2_0 vsSkinnedML(4);       
        
        Sampler[0] = <DiffSampler>;
        Sampler[1] = <EnvironmentSampler>;

        // Load Transparency Map, includes lightmap
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Current;
        ColorArg2[0] = Texture;
        AlphaOp[0]   = SelectArg2;
        AlphaArg1[0] = Current;
        AlphaArg2[0] = Diffuse;     

        // Reflection Texture
        ColorOp[1]   = BlendCurrentAlpha;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1]   = SelectArg1;
        AlphaArg1[1] = Current;

        ColorOp[2]   = Disable;
        AlphaOp[2]   = Disable;
    } 
      
};      


// technique tSkinnedML
// {
//     pass p0
//     {        
//         WorldTransform[0]   = <Model>;
//         //TextureTransform[0] = <TextureTransform0>;
//         //TextureTransform[1] = <Ident>;

//         AlphaRef            = <AlphaRef>;
//         CullMode            = <CullMode>;
//         MaterialDiffuse     = <MatDiffuse>;
//         MaterialEmissive    = <MatEmissive>;        
//         MaterialAmbient     = {1.0, 1.0, 1.0, 1.0};                

//         FVF = XYZ | NORMAL | TEX1;
//         
//         VertexShader = compile vs_2_0 vsSkinnedML(0);
//         
//         TexCoordIndex[0] = 0;
//         //TextureTransformFlags[0] = Count2;
//                 
//         TexCoordIndex[1] = CameraSpaceReflectionVector;
//         //TextureTransformFlags[1] = Count3;
//        
//         
//         Sampler[0] = <DiffSampler>;
//         Sampler[1] = <EnvironmentSampler>;

//         // Base Texture
//         ColorOp[0]   = SelectArg1;
//         ColorArg1[0] = Texture;
//         AlphaOp[0]   = SelectArg1;
//         AlphaArg1[0] = Texture;        

//         // Reflection Texture
//         ColorOp[1]   = BlendCurrentAlpha;
//         ColorArg1[1] = Texture;
//         ColorArg2[1] = Current;
//         AlphaOp[1]   = SelectArg1;
//         AlphaArg1[1] = Current;

//         // Lighting
//         ColorOp[2]   = Modulate;
//         ColorArg1[2] = Diffuse;
//         ColorArg2[2] = Current;
//         AlphaOp[2]   = SelectArg1;
//         AlphaArg1[2] = Current;

//         ColorOp[3]   = Disable;
//         AlphaOp[3]   = Disable;
//     } 
// };      
//------------------------------------------------------------------------------
/**
    Techniques for shader "gui3d"
*/
technique tGui3DColor
{
    pass p0
    {
        CullMode            = <CullMode>;
        SrcBlend            = SRCALPHA;
        DestBlend           = INVSRCALPHA;
        
        TexCoordIndex[0] = 0;
        //TextureTransformFlags[0] = Disable;
        
        VertexShader = compile vs_2_0 vsGui3D();
        
        FVF = XYZ | NORMAL | TEX1;

        Sampler[0] = <DiffSampler>;
   
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        AlphaOp[1] = Disable;
    }
}

//------------------------------------------------------------------------------
/**
*/
technique tUVAnimationAlpha
{
    pass p0
    {        
        VertexShader = compile vs_2_0 vsUvAnimation();
      
        CullMode          = <CullMode>;
        SrcBlend          = <AlphaSrcBlend>;
        DestBlend         = <AlphaDstBlend>;   
        MaterialDiffuse   = <MatDiffuse>;
        MaterialEmissive  = <MatEmissive>;        
        MaterialAmbient   = {1.0, 1.0, 1.0, 1.0};                

        TextureTransform[0] = <Ident>;
        TexCoordIndex[0]    = 0;

        Sampler[0]        = <DiffSampler>;
               
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        
        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;
    }
}
