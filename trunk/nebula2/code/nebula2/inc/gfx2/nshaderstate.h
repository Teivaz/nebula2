#ifndef N_SHADERSTATE_H
#define N_SHADERSTATE_H
//------------------------------------------------------------------------------
/**
    @class nShaderState
    @ingroup NebulaGraphicsSystem

    Shader state parameters and data types.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

namespace nShaderState
{
//------------------------------------------------------------------------------
/**
    Shader parameter data types.
*/
enum Type
{
    Void,
    Bool,
    Int,
    Float,
    Float4,
    Matrix44,
    Texture,
    NumTypes,  // always keep this after all valid types!
    InvalidType
};

//------------------------------------------------------------------------------
/**
    Shader parameters.
    NOTE: don't forget to update the string table in nshaderstate.cc after
    adding or removing shader states!
*/
enum Param
{
    Model = 0,                      // matrix: the model matrix (aka World)
    InvModel,                       // matrix: the inverse model matrix
    View,                           // matrix: the view matrix
    InvView,                        // matrix: the inverse view matrix
    Projection,                     // matrix: the projection matrix
    ModelView,                      // matrix: the model*view matrix
    InvModelView,                   // matrix: the inverse of the model view matrix
    ModelViewProjection,            // matrix: the model*view*projection matrix
    ModelLightProjection,           // matrix: the model*light*projection matrix
    ModelShadowProjection,          // matrix: the model*shadow*projection matrix (shadow == shadow source)
    EyePos,                         // vector: the eye position in world space
    ModelEyePos,                    // vector: the eye position in model space
    ModelLightPos,                  // vector[]: the light position in model space
    LightPos,                       // vector3:  the light position in world space (must be vec3 for the FF pipeline)
    LightPos4,                      // vector4:  the light position in world space (must be vec4 for shadow extrude)
    MatAmbient,                     // color: material ambient component
    MatDiffuse,                     // color: material diffuse component
    MatSpecular,                    // color: material specular component
    MatSpecularPower,               // float: material specular power
    MatTransparency,                // float: material transparency
    MatFresnel,                     // float: material fresnel term
    Scale,                          // float: material scale (for waves, etc...)
    Noise,			                // float: material noise scale (for wavec, etc...)
    MatTranslucency,                // float: material translucency
    AlphaRef,                       // int: alpha ref (0 - 255)
    CullMode,                       // int: cull mode (1 = No culling, 2 = CW, 3 = CCW)
    DirAmbient,                     // color array[6]: directional ambient colors
    FogDistances,                   // vector: x=near, y=far, z=bottom, w=top
    FogNearBottomColor,             // color: rgb, a=lerp
    FogNearTopColor,                // color: rgb, a=lerp
    FogFarBottomColor,              // color: rgb, a=lerp
    FogFarTopColor,                 // color: rgb, a=lerp
    LightAmbient,                   // color[]: light ambient component
    LightDiffuse,                   // color[]: light diffuse component
    LightSpecular,                  // color[]: light specular component
    LightAmbient1,                  // color[]: additional light ambient component
    LightDiffuse1,                  // color[]: additional light diffuse component
    LightSpecular1,                 // color[]: additional light specular component
    DiffMap0,                       // texture: diffuse map layer 0
    DiffMap1,                       // texture: diffuse map layer 1
    DiffMap2,                       // texture: diffuse map layer 2
    DiffMap3,                       // texture: diffuse map layer 3
    SpecMap0,                       // texture: specular map layer 0
    SpecMap1,                       // texture: specular map layer 1
    SpecMap2,                       // texture: specular map layer 2
    SpecMap3,                       // texture: specular map layer 3
    AmbientMap0,                    // texture: ambient map layer 1
    AmbientMap1,                    // texture: ambient map layer 2
    AmbientMap2,                    // texture: ambient map layer 3
    AmbientMap3,                    // texture: ambient map layer 4
    BumpMap0,                       // texture: bump map layer 0
    BumpMap1,                       // texture: bump map layer 1
    BumpMap2,                       // texture: bump map layer 2
    BumpMap3,                       // texture: bump map layer 3
    CubeMap0,                       // texture: cube map layer 0
    CubeMap1,                       // texture: cube map layer 1
    CubeMap2,                       // texture: cube map layer 2
    CubeMap3,                       // texture: cube map layer 3
    NoiseMap0,                      // texture: noise map layer 0
    NoiseMap1,                      // texture: noise map layer 1
    NoiseMap2,                      // texture: noise map layer 2
    NoiseMap3,                      // texture: noise map layer 3
    LightModMap,                    // texture: light modulation map
    ShadowMap,                      // texture: shadow map
    SpecularMap,                    // texture: x^y lookup map for specular highlight
    ShadowModMap,                   // texture: shadow modulation map
    JointPalette,                   // matrix array: joint palette for skinning
    Time,                           // float: current time in seconds
    Wind,                           // vector: the direction and strength
    Swing,                          // matrix: the swing rotation matrix
    InnerLightIntensity,            // float: light intensity at center
    OuterLightIntensity,            // float: light intensity at periphery
    BoxMinPos,                      // vector: bounding box min pos in model space
    BoxMaxPos,                      // vector: bounding box max pos in model space
    BoxCenter,                      // vector: bounding box center in model space
    MinDist,                        // float: a minimum distance
    MaxDist,                        // float: a maximum distance
    SpriteSize,                     // float: size of sprites
    MinSpriteSize,                  // float: minimum sprite size
    MaxSpriteSize,                  // float: maximum sprite size
    SpriteSwingAngle,               // float: swing angle for sprites (rad)
    SpriteSwingTime,                // float: swing time for sprites
    SpriteSwingTranslate,           // float3: sprite swing translation
    DisplayResolution,              // float2: current display width in pixels
    TexGenS,                        // float4: texgen parameter
    TexGenT,                        // float4: texgen parameter
    TexGenR,                        // float4: texgen parameter
    TexGenQ,                        // float4: texgen parameter
    TextureTransform0,              // matrix: the texture matrix for layer 0
    TextureTransform1,              // matrix: the texture matrix for layer 1
    TextureTransform2,              // matrix: the texture matrix for layer 2
    TextureTransform3,              // matrix: the texture matrix for layer 3
    SampleOffsets,                  // float4[]: filter kernel sample offsets
    SampleWeights,                  // float4[]: filter kernel sample weights
    VertexStreams,                  // int: number of parallel vertex streams
    VertexWeights1,                 // float4: weights of streams 1-4
    VertexWeights2,                 // float4: weights of streams 5-8
    AlphaSrcBlend,                  // int: Alpha Source Blend Factor
    AlphaDstBlend,                  // int: Alpha Dest Blend Factor
    BumpScale,                      // float
    FresnelBias,                    // float
    FresnelPower,                   // float
    Intensity0,                     // float
    Intensity1,                     // float
    Intensity2,                     // float
    Intensity3,                     // float
    Amplitude,                      // float
    Frequency,                      // float
    Velocity,                       // float3
    StencilFrontZFailOp,            // int: front faces stencil depth fail operation
    StencilFrontPassOp,             // int: front faces stencil pass operation
    StencilBackZFailOp,             // int: front faces stencil depth fail operation
    StencilBackPassOp,              // int: front faces stencil depth fail operation
    
    NumParameters,                  // keep this always at the end!
    InvalidParameter,
};

enum StencilOperation
{
    KEEP = 1,
    ZERO = 2,
    REPLACE = 3,
    INCRSAT = 4,
    DECRSAT = 5,
    INVERT = 6,
    INCR = 7,
    DECR = 8,
};

enum CullMode
{
    NoCull = 1,
    CW = 2,
    CCW = 3,
};

/// convert type enum to type string
const char* TypeToString(Type t);
/// convert type string to type enum
Type StringToType(const char* str);

/// convert parameter enum to string
const char* ParamToString(Param p);
/// convert string to parameter
Param StringToParam(const char* str);

}; // namespace nShaderState
//------------------------------------------------------------------------------
#endif
