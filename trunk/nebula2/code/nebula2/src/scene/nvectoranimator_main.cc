//------------------------------------------------------------------------------
//  nvectoranimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nvectoranimator.h"
#include "scene/nabstractshadernode.h"
#include "scene/nrendercontext.h"

nNebulaScriptClass(nVectorAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nVectorAnimator::nVectorAnimator() :
    vectorParameter(nShader2::InvalidParameter),
    keyArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVectorAnimator::~nVectorAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set the name of the vector variable which should be animated by
    this object.
*/
void
nVectorAnimator::SetVectorName(const char* name)
{
    n_assert(name);
    this->vectorParameter = nShader2::StringToParameter(name);
}

//------------------------------------------------------------------------------
/**
    Get the name of the vector variable which is animated by this object.
*/
const char*
nVectorAnimator::GetVectorName()
{
    if (nShader2::InvalidParameter == this->vectorParameter)
    {
        return 0;
    }
    else
    {
        return nShader2::ParameterToString(this->vectorParameter);
    }
}

//------------------------------------------------------------------------------
/**
    Add a key to the animation key array.
*/
void
nVectorAnimator::AddKey(float time, const vector4& key)
{
    Key newKey(time, key);
    this->keyArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nVectorAnimator::GetNumKeys() const
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nVectorAnimator::GetKeyAt(int index, float& time, vector4& key) const
{
    const Key& animKey = this->keyArray[index];
    time = animKey.time;
    key  = animKey.value;
}

//------------------------------------------------------------------------------
/**
    Returns the animator type. nVectorAnimator is a SHADER animator.
*/
nAnimator::AnimatorType
nVectorAnimator::GetAnimatorType() const
{
    return SHADER;
}

//------------------------------------------------------------------------------
/**
*/
void
nVectorAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nAbstractShaderNode!
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;

    if (this->keyArray.Size() > 1)
    {
        float minTime = this->keyArray.Front().time;
        float maxTime = this->keyArray.Back().time;
        if (maxTime > 0.0f)
        {
            // get the anim driver value (e.g. the current time)
            nVariable* var = renderContext->GetVariable(this->channelVarHandle);
            n_assert(var);
            float curTime = var->GetFloat();

            if (this->GetLoopType() == LOOP)
            {
                // in loop mode, wrap time into loop time
                curTime = curTime - (float(floor(curTime / maxTime)) * maxTime);
            }

            // clamp time to range
            if (curTime < minTime)       curTime = minTime;
            else if (curTime >= maxTime) curTime = maxTime - 0.001f;

            // find the surrounding keys
            n_assert(this->keyArray.Front().time == 0.0f);
            int i = 0;;
            while (this->keyArray[i].time <= curTime)
            {
                i++;
            }
            n_assert((i > 0) && (i < this->keyArray.Size()));

            const Key& key0 = this->keyArray[i - 1];
            const Key& key1 = this->keyArray[i];
            float time0 = key0.time;
            float time1 = key1.time;

            // compute the actual interpolated values
            float lerp;
            if (time1 > time0) lerp = (float) ((curTime - time0) / (time1 - time0));
            else               lerp = 1.0f;
            vector4 curValue = key0.value + ((key1.value - key0.value) * lerp);
            
            // manipulate the target object
            targetNode->SetVector(this->vectorParameter, curValue);
        }
    }
}
















