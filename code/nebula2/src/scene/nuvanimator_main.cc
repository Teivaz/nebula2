//------------------------------------------------------------------------------
//  nuvanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nuvanimator.h"
#include "scene/nrendercontext.h"
#include "scene/nabstractshadernode.h"

nNebulaScriptClass(nUvAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nUvAnimator::nUvAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nUvAnimator::~nUvAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nUvAnimator::GetAnimatorType() const
{
    return Shader;
}

//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nTransformNode)
    @param  renderContext   current render context
*/
void
nUvAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nAbstractShaderNode!!!
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;

    // get the sample time from the render context
    nVariable* var = renderContext->GetVariable(this->channelVarHandle);
    n_assert(var);
    float curTime = var->GetFloat();

    int texLayer;
    for (texLayer = 0; texLayer < nGfxServer2::MaxTextureStages; texLayer++)
    {
        // sample key arrays and manipulate target object
        static nAnimKey<vector2> key;
        if (this->posArray->Sample(curTime, this->loopType, key))
        {
            targetNode->SetUvPos(texLayer, key.GetValue());
        }
        if (this->eulerArray->Sample(curTime, this->loopType, key))
        {
            targetNode->SetUvEuler(texLayer, key.GetValue());
        }
        if  (this->scaleArray->Sample(curTime, this->loopType, key))
        {
            targetNode->SetUvScale(texLayer, key.GetValue());
        }
    }
}

