//------------------------------------------------------------------------------
//  nmultilayerednode_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmultilayerednode.h"
#include "scene/nsceneserver.h"

nNebulaScriptClass(nMultiLayeredNode, "scene::nshapenode");

//------------------------------------------------------------------------------
/**
*/
nMultiLayeredNode::nMultiLayeredNode()
{
    int i;
    for (i = 0; i < MaxLayers; i++)
    {
        this->uvStretch[i] = 1.0f;
        this->specIntensity[i] = 1.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
nMultiLayeredNode::~nMultiLayeredNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nMultiLayeredNode::ApplyShader(nSceneServer* sceneServer)
{
    nShader2* shd = this->GetShaderObject();
    if (nGfxServer2::Instance()->GetFeatureSet() >= nGfxServer2::DX9)
    {
        // set DX9 shader params
        vector4 dx9UVStretch[2];
        dx9UVStretch[0].set(this->uvStretch[0], this->uvStretch[1], this->uvStretch[2], this->uvStretch[3]);
        dx9UVStretch[1].set(this->uvStretch[4], this->uvStretch[5], this->uvStretch[6], this->uvStretch[7]);
        shd->SetVector4Array(nShaderState::MLPUVStretch, dx9UVStretch, 2);
        vector4 dx9SpecIntensity[2];
        dx9SpecIntensity[0].set(this->specIntensity[0], this->specIntensity[1], this->specIntensity[2], this->specIntensity[3]);
        dx9SpecIntensity[1].set(this->specIntensity[4], this->specIntensity[5], this->specIntensity[6], this->specIntensity[7]);
        shd->SetVector4Array(nShaderState::MLPSpecIntensity, dx9SpecIntensity, 2);
    }
    else
    {
        // set DX7 shader params
        matrix44 uvStretch[MaxLayers];
        int i;
        for (i = 0; i < MaxLayers; i++)
        {
            float stretch = this->uvStretch[i];
            uvStretch[i].ident();
            uvStretch[i].m[0][0] = stretch;
            uvStretch[i].m[1][1] = stretch;
            uvStretch[i].m[2][2] = stretch;
            uvStretch[i].m[3][3] = stretch;
            shd->SetMatrix((nShaderState::Param) (nShaderState::UVStretch0 + i), uvStretch[i]);
        }
    }
    return nShapeNode::ApplyShader(sceneServer);
}

