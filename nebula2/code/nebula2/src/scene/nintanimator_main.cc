//------------------------------------------------------------------------------
//  nintanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nintanimator.h"
#include "scene/nabstractshadernode.h"
#include "scene/nrendercontext.h"

nNebulaScriptClass(nIntAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nIntAnimator::nIntAnimator() :
    vectorParameter(nShaderState::InvalidParameter),
    keyArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nIntAnimator::~nIntAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set the name of the vector variable which should be animated by
    this object.
*/
void
nIntAnimator::SetVectorName(const char* name)
{
    n_assert(name);
    this->vectorParameter = nShaderState::StringToParam(name);
}

//------------------------------------------------------------------------------
/**
    Get the name of the vector variable which is animated by this object.
*/
const char*
nIntAnimator::GetVectorName()
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
nIntAnimator::AddKey(float time, const int& key)
{
    this->keyArray.AddKey(time, key);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nIntAnimator::GetNumKeys() const
{
    return this->keyArray.GetNumKeys();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nIntAnimator::GetKeyAt(int index, float& time, int& key) const
{
    this->keyArray.GetKeyAt(index, time, key);
}

//------------------------------------------------------------------------------
/**
    Returns the animator type. nVectorAnimator is a SHADER animator.
*/
nAnimator::Type
nIntAnimator::GetAnimatorType() const
{
    return Shader;
}

//------------------------------------------------------------------------------
/**
*/
void
nIntAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
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
    int key;
    if (this->keyArray.SampleKey(curTime, key, this->GetLoopType()))
    {
        targetNode->SetInt(this->vectorParameter, key);
    }
}

