#define N_IMPLEMENTS nSkinAnimator
//------------------------------------------------------------------------------
//  nskinanimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nskinanimator.h"
#include "scene/nskinshapenode.h"
#include "anim2/nanimation.h"
#include "anim2/nanimationserver.h"
#include "anim2/nanimstatearray.h"
#include "character/ncharacter2.h"
#include "scene/nrendercontext.h"
#include "variable/nvariableserver.h"

nNebulaScriptClass(nSkinAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nSkinAnimator::nSkinAnimator() :
    refAnimServer(kernelServer, "/sys/servers/anim"),
    animStateVarHandle(nVariable::INVALID_HANDLE),
    frameId(0xffffffff)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSkinAnimator::~nSkinAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Unload the animation resource file.
*/
void
nSkinAnimator::UnloadAnim()
{
    if (this->refAnim.isvalid())
    {
        this->refAnim->Release();
        this->refAnim.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Load the animation resource file.
*/
bool
nSkinAnimator::LoadAnim()
{
    if ((!this->refAnim.isvalid()) && (!this->animName.IsEmpty()))
    {
        const char* fileName = this->animName.Get();
        nAnimation* anim = this->refAnimServer->NewMemoryAnimation(fileName);
        n_assert(anim);
        if (!anim->IsValid())
        {
            anim->SetFilename(fileName);
            if (!anim->Load())
            {
                n_printf("nSkinAnimator: Error loading anim file '%s'\n", fileName);
                anim->Release();
                return false;
            }
        }
        this->refAnim = anim;
        this->character.SetAnimation(anim);
        this->character.SetAnimStateArray(&this->animStateArray);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nSkinAnimator::LoadResources()
{
    if (nSceneNode::LoadResources())
    {
        this->LoadAnim();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nSkinAnimator::UnloadResources()
{
    nSceneNode::UnloadResources();
    this->UnloadAnim();
}

//------------------------------------------------------------------------------
/**
    Check if resources are valid.
*/
bool
nSkinAnimator::AreResourcesValid() const
{
    if (nSceneNode::AreResourcesValid())
    {
        return this->refAnim.isvalid();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::INVALID_HANDLE != this->channelVarHandle);

    // load anim resource if necessary
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    // FIXME: assume that the sceneNode is a nSkinShapeNode
    nSkinShapeNode* skinShapeNode = (nSkinShapeNode*) sceneNode;

    // check if I am already uptodate for this frame
    int curFrameId = renderContext->GetFrameId();
    if (this->frameId != curFrameId)
    {
        this->frameId = curFrameId;

        // get the sample time from the render context
        nVariable* var = renderContext->GetVariable(this->channelVarHandle);
        n_assert(var);
        float curTime = var->GetFloat();

        // get the current anim state from the anim state channel
        // (assume 0 as default state index)
        var = renderContext->GetVariable(this->animStateVarHandle);
        int animState = 0;
        if (var)
        {
            animState = var->GetInt();
        }
        if (animState != this->character.GetActiveState())
        {
            // activate new state
            this->character.SetActiveState(animState, curTime);
        }

        // evaluate the current state of the character skeleton
        this->character.EvaluateSkeleton(curTime, renderContext);
    }

    // update the source node with the new char skeleton state
    skinShapeNode->SetCharSkeleton(&this->character.GetSkeleton());
}

//------------------------------------------------------------------------------
/**
    Begin configuring the joint skeleton.
*/
void
nSkinAnimator::BeginJoints(int numJoints)
{
    this->character.GetSkeleton().Clear();
    this->character.GetSkeleton().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the joint skeleton.
*/
void
nSkinAnimator::SetJoint(int jointIndex, int parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale)
{
    this->character.GetSkeleton().SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale);
}

//------------------------------------------------------------------------------
/**
    Finish adding joints to the joint skeleton.
*/
void
nSkinAnimator::EndJoints()
{
    this->character.GetSkeleton().EndJoints();
}

//------------------------------------------------------------------------------
/**
    Get number of joints in joint skeleton.
*/
int
nSkinAnimator::GetNumJoints()
{
    return this->character.GetSkeleton().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get joint attributes.
*/
void
nSkinAnimator::GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale)
{
    nCharJoint& joint = this->character.GetSkeleton().GetJointAt(index);
    parentJointIndex = joint.GetParentJointIndex();
    poseTranslate = joint.GetPoseTranslate();
    poseRotate    = joint.GetPoseRotate();
    poseScale     = joint.GetPoseScale();
}

//------------------------------------------------------------------------------
/**
    Set name of anim resource file.
*/
void
nSkinAnimator::SetAnim(const char* name)
{
    n_assert(name);
    this->UnloadAnim();
    this->animName = name;
}

//------------------------------------------------------------------------------
/**
    Get name of anim resource file.
*/
const char*
nSkinAnimator::GetAnim() const
{
    return this->animName.IsEmpty() ? 0 : this->animName.Get();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::SetStateChannel(const char* name)
{
    n_assert(name);
    this->animStateVarHandle = this->refVariableServer->GetVariableHandleByName(name);
}

//------------------------------------------------------------------------------
/**
    Return the animation channel which drives this animation.
*/
const char*
nSkinAnimator::GetStateChannel()
{
    if (nVariable::INVALID_HANDLE == this->animStateVarHandle)
    {
        return 0;
    }
    else
    {
        return this->refVariableServer->GetVariableName(this->animStateVarHandle);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::BeginStates(int num)
{
    this->animStateArray.Begin(num);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::SetState(int stateIndex, int animGroupIndex, float fadeInTime)
{
    nAnimState newState;
    newState.SetAnimGroupIndex(animGroupIndex);
    newState.SetFadeInTime(fadeInTime);
    this->animStateArray.SetState(stateIndex, newState);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::EndStates()
{
    this->animStateArray.End();
}

//------------------------------------------------------------------------------
/**
*/
int
nSkinAnimator::GetNumStates() const
{
    return this->animStateArray.GetNumStates();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::GetStateAt(int stateIndex, int& animGroupIndex, float& fadeInTime)
{
    const nAnimState& animState = this->animStateArray.GetStateAt(stateIndex);
    animGroupIndex = animState.GetAnimGroupIndex();
    fadeInTime     = animState.GetFadeInTime();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::BeginClips(int stateIndex, int numClips)
{
    this->animStateArray.GetStateAt(stateIndex).BeginClips(numClips);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::SetClip(int stateIndex, int clipIndex, const char* weightChannelName)
{
    // number of anim curves in a clip is identical to number of (joints * 3)
    // (one curve for translation, rotation and scale)
    int numCurves = this->GetNumJoints() * 3;
    n_assert(numCurves > 0);

    // the first anim curve index of this clip is at (numCurves * clipIndex)
    int firstCurve = numCurves * clipIndex;

    // get the variable handle for the weightChannel
    nVariable::Handle varHandle = this->refVariableServer->GetVariableHandleByName(weightChannelName);
    n_assert(nVariable::INVALID_HANDLE != varHandle);

    nAnimClip newClip(firstCurve, numCurves, varHandle);
    this->animStateArray.GetStateAt(stateIndex).SetClip(clipIndex, newClip);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::EndClips(int stateIndex)
{
    this->animStateArray.GetStateAt(stateIndex).EndClips();
}

//------------------------------------------------------------------------------
/**
*/
int
nSkinAnimator::GetNumClips(int stateIndex) const
{
    return this->animStateArray.GetStateAt(stateIndex).GetNumClips();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::GetClipAt(int stateIndex, int clipIndex, const char*& weightChannelName)
{
    nVariable::Handle varHandle = this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).GetWeightChannelHandle();
    weightChannelName = this->refVariableServer->GetVariableName(varHandle);
}
