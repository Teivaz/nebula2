//------------------------------------------------------------------------------
//  nfloatanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nfloatanimator.h"
#include "scene/nabstractshadernode.h"
#include "scene/nrendercontext.h"

nNebulaScriptClass(nFloatAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nFloatAnimator::nFloatAnimator() :
    vectorParameter(nShaderState::InvalidParameter),
    keyArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nFloatAnimator::~nFloatAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set the name of the vector variable which should be animated by
    this object.
*/
void
nFloatAnimator::SetVectorName(const char* name)
{
    n_assert(name);
    this->vectorParameter = nShaderState::StringToParam(name);
}

//------------------------------------------------------------------------------
/**
    Get the name of the vector variable which is animated by this object.
*/
const char*
nFloatAnimator::GetVectorName()
{
    if (nShaderState::InvalidParameter == this->vectorParameter)
    {
        return 0;
    }
    else
    {
        return nShaderState::ParamToString(this->vectorParameter);
    }
}

//------------------------------------------------------------------------------
/**
    Add a key to the animation key array.
*/
void
nFloatAnimator::AddKey(float time, const float& key)
{
    Key newKey(time, key);
    this->keyArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nFloatAnimator::GetNumKeys() const
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nFloatAnimator::GetKeyAt(int index, float& time, float& key) const
{
    const Key& animKey = this->keyArray[index];
    time = animKey.time;
    key  = animKey.value;
}

//------------------------------------------------------------------------------
/**
    Returns the animator type. nVectorAnimator is a SHADER animator.
*/
nAnimator::Type
nFloatAnimator::GetAnimatorType() const
{
    return Shader;
}

//------------------------------------------------------------------------------
/**
*/
void
nFloatAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nAbstractShaderNode!
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;

    // get the sample time from the render context
    nVariable* var = renderContext->GetVariable(this->channelVarHandle);
    n_assert(var);
    float curTime = var->GetFloat();

    // get sampled key
    float key;
    if (this->SampleKey(curTime, this->keyArray, key))
    {
        targetNode->SetFloat(this->vectorParameter, key);
    }
}

//------------------------------------------------------------------------------
/**
    Compute an interpolated key value from a time and a key array.

    @param  sampleTime  [in] time position at which to sample
    @param  keyArray    [in] reference of a key array containing the keys
    @param  result      [out] the result
    @return             true if the result is valid, false otherwise
*/
bool
nFloatAnimator::SampleKey(float sampleTime, const nArray<Key>& keyArray, float& result)
{
    if (keyArray.Size() > 1)
    {
        float minTime = keyArray.Front().time;
        float maxTime = keyArray.Back().time;
        if (maxTime > 0.0f)
        {
            if (this->GetLoopType() == Loop)
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
