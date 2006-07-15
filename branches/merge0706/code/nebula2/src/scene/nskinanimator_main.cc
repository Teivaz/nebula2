//------------------------------------------------------------------------------
//  nskinanimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nskinanimator.h"
#include "scene/nskinshapenode.h"
#include "scene/nshadowskinshapenode.h"
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
    refAnimServer("/sys/servers/anim"),
    characterVarIndex(0),
    animStateVarHandle(nVariable::InvalidHandle),
    animEnabled(true)
{
    this->skinShapeNodeClass = kernelServer->FindClass("nskinshapenode");
    this->shadowSkinShapeNodeClass = kernelServer->FindClass("nshadowskinshapenode");
    n_assert(this->skinShapeNodeClass);
    n_assert(this->shadowSkinShapeNodeClass);
    this->animRestartVarHandle = nVariableServer::Instance()->GetVariableHandleByName("chnRestartAnim");
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
        nAnimation* anim = this->refAnimServer->NewMemoryAnimation(this->animName);
        n_assert(anim);
        if (!anim->IsLoaded())
        {
            anim->SetFilename(this->animName);
            if (!anim->Load())
            {
                n_printf("nSkinAnimator: Error loading anim file '%s'\n", this->animName.Get());
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
*/
void
nSkinAnimator::RenderContextCreated(nRenderContext* renderContext)
{
    nAnimator::RenderContextCreated(renderContext);

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    nCharacter2* curCharacter = n_new(nCharacter2(this->character));
    n_assert(curCharacter);

    // put frame persistent data in render context
    nVariable::Handle charHandle = nVariableServer::Instance()->GetVariableHandleByName("charPointer");
    this->characterVarIndex = renderContext->AddLocalVar(nVariable(charHandle, curCharacter));
}

//------------------------------------------------------------------------------
/**
    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
void
nSkinAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);    

    const nVariable& charVar = renderContext->GetLocalVar(this->characterVarIndex);
    nCharacter2* curCharacter = (nCharacter2*) charVar.GetObj();
    n_assert(curCharacter);

    // update the animation enabled flag
    //curCharacter->SetAnimEnabled(this->animEnabled);

    // check if I am already uptodate for this frame
    uint curFrameId = renderContext->GetFrameId();
    if (curCharacter->GetLastEvaluationFrameId() != curFrameId)
    {
        curCharacter->SetLastEvaluationFrameId(curFrameId);

        // get the sample time from the render context
        nVariable* var = renderContext->GetVariable(this->channelVarHandle);
        if (var == 0)
        {
            n_error("nSkinAnimator::Animate: TimeChannel Variable '%s' not found in the RenderContext!\n", nVariableServer::Instance()->GetVariableName(this->channelVarHandle));
        }
        float curTime = var->GetFloat();

        // get the time offset from the render context
        var = renderContext->GetVariable(this->channelOffsetVarHandle);
        float curOffset = 0 != var ? var->GetFloat() : 0.0f;

        // get the current anim state from the anim state channel
        // (assume 0 as default state index)
        var = renderContext->GetVariable(this->animStateVarHandle);
        int animState = 0;
        if (var)
        {
            animState = var->GetInt();
        }
        int restartAnim = 0;
        var = renderContext->GetVariable(this->animRestartVarHandle);
        if (var)
        {
            restartAnim = var->GetInt();
        }
        if ((animState != curCharacter->GetActiveState()) || (0 != restartAnim))
        {
            // activate new state
            if (curCharacter->IsValidStateIndex(animState))
            {
                curCharacter->SetActiveState(animState, curTime, curOffset);
            }
            else
            {
                // n_printf("Warning: Invalid state index %d. State switch ignored. \n", animState);
            }
        }

        // evaluate the current state of the character skeleton
        curCharacter->EvaluateSkeleton(curTime, renderContext);
    }

    // update the source node with the new char skeleton state
    if (sceneNode->IsA(this->skinShapeNodeClass))
    {
        nSkinShapeNode* skinShapeNode = (nSkinShapeNode*) sceneNode;
        skinShapeNode->SetCharSkeleton(&curCharacter->GetSkeleton());

    }
    else if (sceneNode->IsA(this->shadowSkinShapeNodeClass))
    {
        nShadowSkinShapeNode* shadowSkinShapeNode = (nShadowSkinShapeNode*) sceneNode;
        shadowSkinShapeNode->SetCharSkeleton(&curCharacter->GetSkeleton());
    }
    else
    {
        n_error("nSkinAnimator::Animate(): invalid scene node class\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::RenderContextDestroyed(nRenderContext* renderContext)
{
    nVariable var;
    var = renderContext->GetLocalVar(this->characterVarIndex);
    nCharacter2* curCharacter = (nCharacter2*) var.GetObj();
    n_assert(curCharacter);
    curCharacter->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinAnimator::SetStateChannel(const char* name)
{
    this->animStateVarHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
}

//------------------------------------------------------------------------------
/**
    Returns the name of the animation variable which drives this animation. 
    This variable exists within the variable server located at 
    @c /sys/servers/variable.
*/
const char*
nSkinAnimator::GetStateChannel()
{
    if (nVariable::InvalidHandle == this->animStateVarHandle)
    {
        return 0;
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->animStateVarHandle);
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
    @note Be sure that you should create a new animation state first by calling 
          nSkinAnimator::SetState() before you calling this function.
*/
void
nSkinAnimator::SetStateName(int stateIndex, const nString& name)
{
    this->animStateArray.GetStateAt(stateIndex).SetName(name);
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
const nAnimState&
nSkinAnimator::GetStateAt(int stateIndex)
{
    return this->animStateArray.GetStateAt(stateIndex);
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
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName(weightChannelName);
    n_assert(nVariable::InvalidHandle != varHandle);

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
    weightChannelName = nVariableServer::Instance()->GetVariableName(varHandle);
}

//------------------------------------------------------------------------------
/**
    Begin adding animation event tracks to a clip.
*/
void
nSkinAnimator::BeginAnimEventTracks(int stateIndex, int clipIndex, int numTracks)
{
    this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).SetNumAnimEventTracks(numTracks);
}

//------------------------------------------------------------------------------
/**
    Begin adding events to an animation event track.
*/
void
nSkinAnimator::BeginAnimEventTrack(int stateIndex, int clipIndex, int trackIndex, const nString& name, int numEvents)
{
    nAnimEventTrack& t = this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).GetAnimEventTrackAt(trackIndex);
    t.SetName(name);
    t.SetNumEvents(numEvents);
}

//------------------------------------------------------------------------------
/**
    Set an animation event in a track
*/
void
nSkinAnimator::SetAnimEvent(int stateIndex, int clipIndex, int trackIndex, int eventIndex, float time, const vector3& translate, const quaternion& rotate, const vector3& scale)
{
    nAnimEventTrack& t = this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).GetAnimEventTrackAt(trackIndex);
    nAnimEvent e;
    e.SetTime(time);
    e.SetTranslation(translate);
    e.SetQuaternion(rotate);
    e.SetScale(scale);
    t.SetEvent(eventIndex, e);
}

//------------------------------------------------------------------------------
/**
    End adding animation events.
*/
void
nSkinAnimator::EndAnimEventTrack(int /*stateIndex*/, int /*clipIndex*/, int /*trackIndex*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    End adding animation event tracks.
*/
void
nSkinAnimator::EndAnimEventTracks(int /*stateIndex*/, int /*clipIndex*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Get the index of the nCharacter2 pointer used when accessing the render context in Animate()

    @return the requested index into the render context
*/
int
nSkinAnimator::GetCharacterVarIndex()
{
    return this->characterVarIndex;
}
