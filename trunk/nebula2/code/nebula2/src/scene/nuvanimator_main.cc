//------------------------------------------------------------------------------
//  nuvanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nuvanimator.h"
#include "scene/nrendercontext.h"
#include "scene/nabstractshadernode.h"

nNebulaScriptClass(nUvAnimator, "nkeyanimator");

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
        static vector4 key4;
        static vector2 key2;
        if (this->SampleKey(curTime, this->posArray[texLayer], key4))
        {
            key2.set(key4.x, key4.y);
            targetNode->SetUvPos(texLayer, key2);
        }
        if (this->SampleKey(curTime, this->eulerArray[texLayer], key4))
        {
            key2.set(key4.x, key4.y);
            targetNode->SetUvEuler(texLayer, key2);
        }
        if (this->SampleKey(curTime, this->scaleArray[texLayer], key4))
        {
            key2.set(key4.x, key4.y);
            targetNode->SetUvScale(texLayer, key2);
        }
    }
}

