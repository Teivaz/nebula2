//------------------------------------------------------------------------------
//  nshaderstate.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nshaderstate.h"
#include <string.h>

/// the shader parameter name string table
static const char* StateTable[nShaderState::NumParameters] = 
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
    "EyePos",
    "ModelEyePos",
    "ModelLightPos",
    "LightPos",
    "LightPos4",
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
    "LightAmbient1",
    "LightDiffuse1",
    "LightSpecular1",
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
    "Velocity",
    "StencilFrontZFailOp",
    "StencilFrontPassOp",
    "StencilBackZFailOp",
    "StencilBackPassOp"
};

//------------------------------------------------------------------------------
/**
*/
const char*
nShaderState::ParamToString(nShaderState::Param p)
{
    n_assert((p >= 0) && (p < nShaderState::NumParameters));
    return StateTable[p];
}

//------------------------------------------------------------------------------
/**
*/
nShaderState::Param
nShaderState::StringToParam(const char* str)
{
    n_assert(str);
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        if (0 == strcmp(str, StateTable[i]))
        {
            return (nShaderState::Param) i;
        }
    }
    // fallthrough: state not found 
    return nShaderState::InvalidParameter;
}
