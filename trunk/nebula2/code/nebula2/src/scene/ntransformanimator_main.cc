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
    float curTime = 0.0f;
    if (0 == var)
    {
        n_printf("Warning: nTransformAnimator::Animate() ChannelVariable '%s' not found!\n", this->GetChannel());
    }
    else
    {
        curTime = var->GetFloat();
    }

    // sample key arrays and manipulate target object
    const nAnimator::LoopType loopType = this->GetLoopType();
    vector3 key3;
    quaternion quat;
    if (this->posArray.SampleKey(curTime, key3, loopType))
    {
        targetNode->SetPosition(key3);
    }
    if (this->quatArray.SampleKey(curTime, quat, loopType))
    {
        targetNode->SetQuat(quat);
    }
    if (this->eulerArray.SampleKey(curTime, key3, loopType))
    {
        targetNode->SetEuler(key3);
    }
    if (this->scaleArray.SampleKey(curTime, key3, loopType))
    {
        targetNode->SetScale(key3);
    }
}
