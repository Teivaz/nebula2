//------------------------------------------------------------------------------
//  nshader2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nshader2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nShader2, "nresource");

/// the shader parameter name string table
const char* nShader2::StringTable[nShader2::NumParameters] = 
{
    "Model",
    "View",
    "Projection",
    "ModelView",
    "InvModelView",
    "ModelViewProjection",
    "ModelLightProjection",
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
    "Frequency",
    "Height",
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
    "TextureTransform3"
};

//------------------------------------------------------------------------------
/**
*/
nShader2::nShader2()
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
nShader2::IsParameterUsed(Parameter p)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetInt(Parameter p, int val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetIntArray(Parameter p, const int* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat(Parameter p, float val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloatArray(Parameter p, const float* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector4(Parameter p, const vector4& val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector3(Parameter p, const vector3& val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat4(Parameter p, const nFloat4& val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat4Array(Parameter p, const nFloat4* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector4Array(Parameter p, const vector4* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrix(Parameter p, const matrix44& val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixArray(Parameter p, const matrix44* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixPointerArray(Parameter p, const matrix44** array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetTexture(Parameter p, nTexture2* val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetParams(const nShaderParams& params)
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

