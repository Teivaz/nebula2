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
    this->keyArray.AddKey(time, key);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nVectorAnimator::GetNumKeys() const
{
    return this->keyArray.GetNumKeys();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nVectorAnimator::GetKeyAt(int index, float& time, vector4& key) const
{
    this->keyArray.GetKeyAt(index, time, key);
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

            // get the anim driver value (e.g. the current time)
            nVariable* var = renderContext->GetVariable(this->channelVarHandle);
            n_assert(var);
            float curTime = var->GetFloat();

    vector4 result;
    if (this->keyArray.SampleKey(curTime, result, this->GetLoopType()))
    {
        // manipulate the target object
        targetNode->SetVector(this->vectorParameter, result);
    }
}
















