//------------------------------------------------------------------------------
//  nlightnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nlightnode.h"
#include "gfx2/nshader2.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nLightNode, "nabstractshadernode");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nlightnode

    @cppclass
    nLightNode
    
    @superclass
    nabstractshadernode
    
    @classinfo
    Generic light.
*/


//------------------------------------------------------------------------------
/**
*/
nLightNode::nLightNode()
{
    // empty
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
    "Render" the light into the current shader.

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server

    @param  sceneServer         pointer to the scene server object rendering this node
    @param  renderContext       pointer to the render context for this node
    @param  lightTransform      this node's model matrix
*/
bool
nLightNode::RenderLight(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& /*lightTransform*/)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    // invoke shader animators
    this->InvokeShaderAnimators(renderContext);

    // get the current shader object from the gfx server
    nShader2* shader = this->refGfxServer->GetShader();
    n_assert(shader);

    // apply int, float and vector variables to the shader
    shader->SetParams(this->shaderParams);

    // apply texture shader variables to the shader
    /*
    int numTextureVariables = this->texNodeArray.Size();
    int i;
    for (i = 0; i < numTextureVariables; i++)
    {
        TexNode& texNode = this->texNodeArray[i];
        if (shader->IsParameterUsed(texNode.shaderParameter))
        {
            shader->SetTexture(texNode.shaderParameter, texNode.refTexture.get());
        }
    }
    */
    return true;
}

