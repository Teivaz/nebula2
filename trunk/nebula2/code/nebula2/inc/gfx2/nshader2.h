#ifndef N_SHADER2_H
#define N_SHADER2_H
//------------------------------------------------------------------------------
/**
    @class nShader2
    @ingroup NebulaGraphicsSystem

    A shader object loads itself from a shader resource file, and contains
    everything to render a mesh and texture. It may be completely
    render state based, use vertex and pixel shader programs, or both.
    Shaders usually use a 3rd party subsystem, like D3DX effects, or CgFX.
    This is done by subclasses of the nShader2 class.

    How the shader is rendered is totally up to the gfx server.

    For the sake of efficiency, shader parameters are now enums with
    associated string names. The use of enums allows to do parameter lookup
    as simple indexed array lookup. The disadvantage is of course, that
    new shader states require this file to be extended or replaced.

    (C) 2002 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "mathlib/matrix.h"
#include "variable/nvariable.h"

class nTexture2;
class nVariableServer;
class nShaderParams;

//------------------------------------------------------------------------------
class nShader2 : public nResource
{
public:
    /// NOTE: don't forget to update the string table in nshader2_main.cc after
    /// adding or removing shader states!
    enum Parameter
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
        ModelEyePos,                    // vector: the eye position in model space
        ModelLightPos,                  // vector[]: the light position in model space
        LightPos,                       // vector[]: the light position in world space
        //LightDirection,                 // vector[]: the light direction in world space
        MatAmbient,                     // color: material ambient component
        MatDiffuse,                     // color: material diffuse component
        MatSpecular,                    // color: material specular component
        MatSpecularPower,               // float: material specular power
        MatTransparency,                // float: material transparency
        MatFresnel,                     // float: material fresnel term
        //Frequency,                      // float: material frequency (for waves, etc...)
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

        NumParameters,                  // keep this always at the end!
        InvalidParameter,
    };

    /// constructor
    nShader2();
    /// destructor
    virtual ~nShader2();
    /// get shader priority index (valid after Load())
    int GetShaderIndex() const;
    /// set a technique
    virtual bool SetTechnique(const char* t);
    /// get current technique
    virtual const char* GetTechnique() const;
    /// is parameter used by effect?
    virtual bool IsParameterUsed(Parameter p);
    /// set bool parameter
    //virtual void SetBool(Parameter p, bool val);
    /// set int parameter
    virtual void SetInt(Parameter p, int val);
    /// set float parameter
    virtual void SetFloat(Parameter p, float val);
    /// set vector parameter as vector4 (fastest)
    virtual void SetVector4(Parameter p, const vector4& val);
    /// set vector parameter as vector3
    virtual void SetVector3(Parameter p, const vector3& val);
    /// set vector parameter as nFloat4
    virtual void SetFloat4(Parameter p, const nFloat4& val);
    /// set matrix parameter
    virtual void SetMatrix(Parameter p, const matrix44& val);
    /// set texture parameter
    virtual void SetTexture(Parameter p, nTexture2* tex);

    /// set bool[] parameter
    //virtual void SetBoolArray(Parameter p, const bool* array, int count);
    /// set int[] parameter
    virtual void SetIntArray(Parameter p, const int* array, int count);
    /// set float[] parameter
    virtual void SetFloatArray(Parameter p, const float* array, int count);
    /// set float4[] parameter
    virtual void SetFloat4Array(Parameter p, const nFloat4* array, int count);
    /// set vector4[] parameter
    virtual void SetVector4Array(Parameter p, const vector4* array, int count);
    /// set matrix array parameter
    virtual void SetMatrixArray(Parameter p, const matrix44* array, int count);
    /// set matrix pointer array parameter
    virtual void SetMatrixPointerArray(Parameter p, const matrix44** array, int count);
    /// set a whole shader parameter block at once
    virtual void SetParams(const nShaderParams& params);

    /// begin applying the shader, returns number of passes
    virtual int Begin();
    /// render a pass
    virtual void Pass(int pass);
    /// finish applying the shader
    virtual void End();
 
    /// convert shader state enum to string
    static const char* ParameterToString(Parameter p);
    /// convert string to shader state
    static Parameter StringToParameter(const char* str);

protected:
    static const char* StringTable[NumParameters];
    int shaderIndex;
};

//------------------------------------------------------------------------------
/**
    Returns the global shader priority index (as defined in
    "shaders:shaderlist.txt"). Only valid between Load()/Unload()!
*/
inline
int
nShader2::GetShaderIndex() const
{
    return this->shaderIndex;
}

//------------------------------------------------------------------------------
/**
    Convert a shader state enum value to a string.
*/
inline
const char*
nShader2::ParameterToString(Parameter p)
{
    n_assert((p >= 0) && (p < NumParameters));
    return StringTable[p];
}

//------------------------------------------------------------------------------
/**
    Convert a string to a shader state enum value.
*/
inline
nShader2::Parameter
nShader2::StringToParameter(const char* str)
{
    n_assert(str);
    int i;
    for (i = 0; i < NumParameters; i++)
    {
        if (0 == strcmp(str, StringTable[i]))
        {
            return (Parameter) i;
        }
    }
    //n_printf("WARNING: '%s' it not a valid/supported ShaderParameter!\n", str);
    // fallthrough: state not found
    return InvalidParameter;
}

//------------------------------------------------------------------------------
#endif
