//------------------------------------------------------------------------------
//  nanimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nanimator.h"
#include "variable/nvariableserver.h"

nNebulaScriptClass(nAnimator, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nAnimator::nAnimator() :
    loopType(Loop)
{
    this->channelVarHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::~nAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Returns the type of the animator object. Subclasses should return
    something meaningful here.
*/
nAnimator::Type
nAnimator::GetAnimatorType() const
{
    return InvalidType;
}

//------------------------------------------------------------------------------
/**
    This method is called back by scene node objects which wish to be
    animated. 
*/
void
nAnimator::Animate(nSceneNode* /*sceneNode*/, nRenderContext* /*renderContext*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Sets the "animation channel" which drives this animation.
    This could be something like "time", but the actual names are totally
    up to the application. The actual channel value will be pulled from
    the render context provided in the Animate() method.
*/
void
nAnimator::SetChannel(const char* name)
{
    n_assert(name);
    this->channelVarHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
}

//------------------------------------------------------------------------------
/**
    Return the animation channel which drives this animation.
*/
const char*
nAnimator::GetChannel()
{
    if (nVariable::InvalidHandle == this->channelVarHandle)
    {
        return 0;
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->channelVarHandle);
    }
}

//------------------------------------------------------------------------------
/**
    Set the loop type for this animation.
*/
void
nAnimator::SetLoopType(LoopType t)
{
    this->loopType = t;
}

//------------------------------------------------------------------------------
/**
    Get the loop type for this animation.
*/
nAnimator::LoopType
nAnimator::GetLoopType() const
{
    return this->loopType;
}

