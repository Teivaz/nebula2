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

    const nAnimator::LoopType loopType = this->GetLoopType();
    int texLayer;
    for (texLayer = 0; texLayer < nGfxServer2::MaxTextureStages; texLayer++)
    {
        // sample key arrays and manipulate target object
        vector2 key;
        if (this->posArray[texLayer].SampleKey(curTime, key, loopType))
        {
            targetNode->SetUvPos(texLayer, key);
        }
        if (this->eulerArray[texLayer].SampleKey(curTime, key, loopType))
        {
            targetNode->SetUvEuler(texLayer, key);
        }
        if (this->scaleArray[texLayer].SampleKey(curTime, key, loopType))
        {
            targetNode->SetUvScale(texLayer, key);
        }
    }
}

