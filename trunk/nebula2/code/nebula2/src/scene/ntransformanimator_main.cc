//------------------------------------------------------------------------------
//  ntransformanimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformanimator.h"
#include "scene/nrendercontext.h"
#include "scene/ntransformnode.h"

nNebulaScriptClass(nTransformAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nTransformAnimator::nTransformAnimator() :
    posArray(0, 4),
    eulerArray(0, 4),
    scaleArray(0, 4),
    quatArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformAnimator::~nTransformAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nTransformAnimator::GetAnimatorType() const
{
    return Transform;
}

//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nTransformNode)
    @param  renderContext   current render context
*/
void
nTransformAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nTransformNode
    nTransformNode* targetNode = (nTransformNode*) sceneNode;

    // get the sample time from the render context
    nVariable* var = renderContext->GetVariable(this->channelVarHandle);
    n_assert(var);
    float curTime = var->GetFloat();

    // sample key arrays and manipulate target object
    vector3 vectorKey;
    quaternion quatKey;
    if (this->posArray.SampleKey(curTime, vectorKey, this->GetLoopType()))
    {
        targetNode->SetPosition(vectorKey);
    }
    
    if (this->eulerArray.SampleKey(curTime, vectorKey, this->GetLoopType()))
    {
        targetNode->SetEuler(vectorKey);
    }
    else if (this->quatArray.SampleKey(curTime, quatKey, this->GetLoopType()))
    {
        targetNode->SetQuat(quatKey);
    }
    
    if (this->scaleArray.SampleKey(curTime, vectorKey, this->GetLoopType()))
    {
        targetNode->SetScale(vectorKey);
    }
}
