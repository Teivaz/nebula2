//------------------------------------------------------------------------------
//  nshader2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nshader2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nShader2, "nresource");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nshader2

    @cppclass
    nShader2
    
    @superclass
    nresource
    
    @classinfo
    Docs needed.
*/
//------------------------------------------------------------------------------

/// the shader parameter name string table
const char* nShader2::StringTable[nShader2::NumParameters] = 
{
    "Model",
    "InvModel",
    "View",
    "InvView",
    "Projection",
    "ModelView",
    "InvModelView",
    "ModelViewProjection",
    "ModelLightProjection",
    "ModelShadowProjection",
    "ModelEyePos",
    "ModelLightPos",
    "LightPos",
    "LightDirection",
    "MatAmbient",
    "MatDiffuse",
    "MatSpecular",
    "MatSpecularPower",
    "MatTransparency",
    "MatFresnel",
    "Scale",
    "Noise",
    "MatTranslucency",
    "AlphaRef",
    "CullMode",
    "DirAmbient",
    "FogDistances",
    "FogNearBottomColor",
    "FogNearTopColor",
    "FogFarBottomColor",
    "FogFarTopColor",
    "LightAmbient",
    "LightDiffuse",
    "LightSpecular",
    "DiffMap0",
    "DiffMap1",
    "DiffMap2",
    "DiffMap3",
    "SpecMap0",
    "SpecMap1",
    "SpecMap2",
    "SpecMap3",
    "AmbientMap0",
    "AmbientMap1",
    "AmbientMap2",
    "AmbientMap3",
    "BumpMap0",
    "BumpMap1",
    "BumpMap2",
    "BumpMap3",
    "CubeMap0",
    "CubeMap1",
    "CubeMap2",
    "CubeMap3",
    "NoiseMap0",
    "NoiseMap1",
    "NoiseMap2",
    "NoiseMap3",
    "LightModMap",
    "ShadowMap",
    "SpecularMap",
    "ShadowModMap",
    "JointPalette",
    "Time",
    "Wind",
    "Swing",
    "InnerLightIntensity",
    "OuterLightIntensity",
    "BoxMinPos",
    "BoxMaxPos",
    "BoxCenter",
    "MinDist",
    "MaxDist",
    "SpriteSize",
    "MinSpriteSize",
    "MaxSpriteSize",
    "SpriteSwingAngle",
    "SpriteSwingTime",
    "SpriteSwingTranslate",
    "DisplayResolution",
    "TexGenS",
    "TexGenT",
    "TexGenR",
    "TexGenQ",
    "TextureTransform0",
    "TextureTransform1",
    "TextureTransform2",
    "TextureTransform3",
    "SampleOffsets", 
    "SampleWeights",
    "VertexStreams",
    "VertexWeights1",
    "VertexWeights2",
    "AlphaSrcBlend",
    "AlphaDstBlend",
    "BumpScale",
    "FresnelBias",
    "FresnelPower",
    "Intensity0",
    "Intensity1",
    "Intensity2",
    "Intensity3",
    "Amplitude",
    "Frequency",
    "Velocity"
};

//------------------------------------------------------------------------------
/**
*/
nShader2::nShader2() :
    shaderIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShader2::~nShader2()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nShader2::IsParameterUsed(Parameter /*p*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetBool(Parameter /*p*/, bool /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetBoolArray(Parameter /*p*/, const bool* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetInt(Parameter /*p*/, int /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetIntArray(Parameter /*p*/, const int* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat(Parameter /*p*/, float /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloatArray(Parameter /*p*/, const float* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector4(Parameter /*p*/, const vector4& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector3(Parameter /*p*/, const vector3& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat4(Parameter /*p*/, const nFloat4& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat4Array(Parameter /*p*/, const nFloat4* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector4Array(Parameter /*p*/, const vector4* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrix(Parameter /*p*/, const matrix44& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixArray(Parameter /*p*/, const matrix44* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixPointerArray(Parameter /*p*/, const matrix44** /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetTexture(Parameter /*p*/, nTexture2* /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetParams(const nShaderParams& /*params*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
nShader2::Begin()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::Pass(int /*pass*/)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
nShader2::End()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nShader2::SetTechnique(const char* /*t*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nShader2::GetTechnique() const
{
    return 0;
}


