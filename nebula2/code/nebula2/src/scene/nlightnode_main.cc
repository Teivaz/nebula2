//------------------------------------------------------------------------------
//  nlightnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nlightnode.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nLightNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nLightNode::nLightNode()
{
    this->light.SetType(nLight::Directional);
}

//------------------------------------------------------------------------------
/**
*/
nLightNode::~nLightNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nLightNode::HasLight() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Setup light parameters once before rendering instances of scene nodes
    influenced by this light.
*/
bool
nLightNode::RenderLight(nSceneServer* sceneServer, nRenderContext* /*renderContext*/, const matrix44& lightTransform)
{
    n_assert(sceneServer);

    // FIXME?
    this->light.SetTransform(lightTransform);
    this->light.SetDiffuse(this->shaderParams.GetArg(nShaderState::LightDiffuse).GetVector4());
    this->light.SetSpecular(this->shaderParams.GetArg(nShaderState::LightSpecular).GetVector4());
    this->light.SetAmbient(this->shaderParams.GetArg(nShaderState::LightAmbient).GetVector4());
    this->light.SetSecondaryDiffuse(this->shaderParams.GetArg(nShaderState::LightDiffuse1).GetVector4());
    this->light.SetSecondarySpecular(this->shaderParams.GetArg(nShaderState::LightSpecular1).GetVector4());
    nGfxServer2::Instance()->AddLight(this->light);

    return true;
}
