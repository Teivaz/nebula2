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
    scaleArray(0, 4)
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
nAnimator::AnimatorType
nTransformAnimator::GetAnimatorType() const
{
    return TRANSFORM;
}

//------------------------------------------------------------------------------
/**
    Compute an interpolated key value from a time and a key array.

    @param  sampleTime  [in] time position at which to sample
    @param  keyArray    [in] reference of a key array containing the keys
    @param  result      [out] the result
    @param              true if the result is valid, false otherwise
*/
bool
nTransformAnimator::SampleKey(float sampleTime, const nArray<Key>& keyArray, vector3& result)
{
    if (keyArray.Size() > 1)
    {
        float minTime = keyArray.Front().time;
        float maxTime = keyArray.Back().time;
        if (maxTime > 0.0f)
        {
            if (this->GetLoopType() == LOOP)
            {
                // in loop mode, wrap time into loop time
                sampleTime = sampleTime - (float(floor(sampleTime / maxTime)) * maxTime);
            }

            // clamp time to range
            if (sampleTime < minTime)       sampleTime = minTime;
            else if (sampleTime >= maxTime) sampleTime = maxTime - 0.001f;

            // find the surrounding keys
            n_assert(keyArray.Front().time == 0.0f);
            int i = 0;;
            while (keyArray[i].time <= sampleTime)
            {
                i++;
            }
            n_assert((i > 0) && (i < keyArray.Size()));

            const Key& key0 = keyArray[i - 1];
            const Key& key1 = keyArray[i];
            float time0 = key0.time;
            float time1 = key1.time;

            // compute the actual interpolated values
            float lerp;
            if (time1 > time0) lerp = (float) ((sampleTime - time0) / (time1 - time0));
            else               lerp = 1.0f;
            result = key0.value + ((key1.value - key0.value) * lerp);
            return true;
        }
    }
    return false;
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
    vector3 key;
    if (this->SampleKey(curTime, this->posArray, key))
    {
        targetNode->SetPosition(key);
    }
    if (this->SampleKey(curTime, this->eulerArray, key))
    {
        targetNode->SetEuler(key);
    }
    if (this->SampleKey(curTime, this->scaleArray, key))
    {
        targetNode->SetScale(key);
    }
}
