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
    this->keyArray.AddKey(time, key);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nFloatAnimator::GetNumKeys() const
{
    return this->keyArray.GetNumKeys();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nFloatAnimator::GetKeyAt(int index, float& time, float& key) const
{
    this->keyArray.GetKeyAt(index, time, key);
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
    if (this->keyArray.SampleKey(curTime, key, this->GetLoopType()))
    {
        targetNode->SetFloat(this->vectorParameter, key);
    }
}

