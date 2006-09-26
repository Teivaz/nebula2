//------------------------------------------------------------------------------
//  graphics/charentity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/charentity.h"
#include "variable/nvariableserver.h"
#include "character/ncharacter2.h"
#include "graphics/server.h"
#include "scene/ncharacter3node.h"
#include "scene/nskinanimator.h"
#include "kernel/nfileserver2.h"

namespace Graphics
{
ImplementRtti(Graphics::CharEntity, Graphics::Entity);
ImplementFactory(Graphics::CharEntity);

//------------------------------------------------------------------------------
/**
*/
CharEntity::CharEntity() :
    baseAnimStarted(0.0),
    baseAnimOffset(0.0),
    baseAnimDuration(0.0),
    baseAnimFadeIn(0.0),
    overlayAnimStarted(0.0),
    overlayAnimDuration(0.0),
    restartOverlayAnim(0),
    overlayEndFadeIn(0.0),
    nebCharacter(0),
    characterSet(0),
    animEventHandler(0),
    character3Mode(false),
    character3NodePtr(0),
    activateNewBaseAnim(false)
{
    // initialize Nebula variable handles
    nVariableServer* varServer  = nVariableServer::Instance();
    this->animRestartVarHandle  = varServer->GetVariableHandleByName("chnRestartAnim");
    this->animOffsetVarHandle   = varServer->GetVariableHandleByName("timeOffset");
    this->characterHandle       = varServer->GetVariableHandleByName("charPointer");
    this->characterSetHandle    = varServer->GetVariableHandleByName("charSetPointer");
    this->renderContext.AddVariable(nVariable(this->animOffsetVarHandle, 0.0f));
    this->renderContext.AddVariable(nVariable(this->animRestartVarHandle, 0));
}

//------------------------------------------------------------------------------
/**
*/
CharEntity::~CharEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::OnActivate()
{
    n_assert(0 == this->nebCharacter);
    n_assert(0 == this->animEventHandler);

    Entity::OnActivate();

    // create animation event handler
    this->CreateAnimationEventHandler();
    n_assert(0 != this->animEventHandler);

    // lookup character pointer
    nVariable* var = this->renderContext.FindLocalVar(this->characterHandle);
    if (var)
    {
        this->nebCharacter = (nCharacter2*) var->GetObj();
        n_assert(this->nebCharacter);
        this->nebCharacter->SetAnimEventHandler(this->animEventHandler);
    }
    this->animEventHandler->SetEntity(this);

    // lookup character set
    this->characterSetHandle = this->nebCharacter->GetSkinAnimator()->GetCharacterSetIndexHandle();
    const nVariable& characterSetVar = this->renderContext.GetLocalVar(this->characterSetHandle);
    this->characterSet = (nCharacter2Set*) characterSetVar.GetObj();

    nClass* nCharacter3SkinAnimatorClass = nKernelServer::Instance()->FindClass("ncharacter3skinanimator");
    n_assert(0 != nCharacter3SkinAnimatorClass);
    if (this->nebCharacter->GetSkinAnimator()->IsA(nCharacter3SkinAnimatorClass))
    {
        nClass* nCharacter3NodeClass = nKernelServer::Instance()->FindClass("ncharacter3node");
        n_assert(0 != nCharacter3NodeClass);
        nTransformNode* charParentNode = this->GetResource().GetNode();
        n_assert(0 != charParentNode);
        nCharacter3Node* charNode = (nCharacter3Node*) this->FindFirstInstance(charParentNode, nCharacter3NodeClass);
        n_assert(0 != charNode);

        this->character3NodePtr = charNode;
        this->character3Mode = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::OnDeactivate()
{
    n_assert(this->animEventHandler);

    this->animEventHandler->SetEntity(0);
    if (this->nebCharacter)
    {
        this->nebCharacter->SetAnimEventHandler(0);
        this->nebCharacter = 0;
    }

    this->CleanupAnimationEventHandler();

    Entity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
int
CharEntity::GetNumAnimations() const
{
    if (this->nebCharacter)
    {
        nSkinAnimator* skinAnimator = this->nebCharacter->GetSkinAnimator();
        n_assert(0 != skinAnimator);
        return skinAnimator->GetNumClips();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Set a new base animation. This is usually a looping animation, like
    Idle, Walking, Running, etc...

    @param  animName            new base animation
    @param  fadeIn              time to fade from current animation
    @param  timeOffset          optional animation time offset
    @param  onlyIfInactive
*/
void
CharEntity::SetBaseAnimation(const nString& animName, nTime fadeIn, nTime timeOffset, bool onlyIfInactive)
{
    n_assert(animName.IsValid());

    if (onlyIfInactive)
    {
        if (!this->baseAnimNames.Empty() && this->baseAnimNames[0] == animName)
        {
            return;
        }
    }

    nArray<nString> anims;
    anims.PushBack(animName);
    nArray<float> weights;
    weights.PushBack(1.0f);

    this->SetBaseAnimationMix(anims, weights, fadeIn, timeOffset);
}

//------------------------------------------------------------------------------
/**
    Set a new base animation. This is usually a looping animation, like
    Idle, Walking, Running, etc...

    @param  animNames           new base animations
    @param  animWeights         new base animation weights
    @param  fadeIn              time to fade from current animation
    @param  timeOffset          optional animation time offset
*/
void
CharEntity::SetBaseAnimationMix(const nArray<nString>& animNames, const nArray<float>& animWeights, nTime fadeIn, nTime timeOffset)
{
    n_assert(animNames.Size() >= 1);
    n_assert(animWeights.Size() >= 1);
    n_assert(this->nebCharacter);
    n_assert(0 != this->characterSet);

    nSkinAnimator* skinAnimator = this->nebCharacter->GetSkinAnimator();
    n_assert(0 != skinAnimator);

    const nString& mappedName = AnimTable::Instance()->Lookup(this->animMapping, animNames[0]).GetAnimName();
    int index = skinAnimator->GetClipIndexByName(mappedName);
    if (-1 != index)
    {
        nTime duration = skinAnimator->GetClipDuration(index);

        this->baseAnimNames = animNames;
        this->baseAnimWeights = animWeights;
        this->baseAnimStarted = this->GetEntityTime();
        this->baseAnimFadeIn = fadeIn;
        nTime dur = 0.0;
        if (this->timeFactor > 0.0f)
        {
            dur = (duration - fadeIn) / this->timeFactor;
        }
        if (dur < 0.0)
        {
            dur = 0.0;
        }
        this->baseAnimDuration = dur;
        this->baseAnimOffset = timeOffset;

        if (!this->IsOverlayAnimationActive())
        {
            this->ActivateAnimations(this->baseAnimNames, this->baseAnimWeights, fadeIn);
        }
        else
        {
            this->activateNewBaseAnim = true;
        }
    }
    else
    {
#ifdef _DEBUG
        n_printf("CharEntity::SetBaseAnimationMix:: AnimColumn: %s AnimRow: %s with table animation: %s in character not found! \n", this->animMapping.Get(), animNames[0].Get(), mappedName.Get());
#endif
    }
}

//------------------------------------------------------------------------------
/**
    Set a new overlay animation. This is usually an one-shot animation, like
    Idle, Walking, Running, etc...

    @param  animName            new base animation
    @param  fadeIn              time to fade from current animation
    @param  overrideDuration    if != 0.0, override the animation's duration with this value
*/
void
CharEntity::SetOverlayAnimation(const nString& animName, nTime fadeIn, nTime overrideDuration, bool onlyIfInactive)
{
    n_assert(animName.IsValid());

    if (onlyIfInactive)
    {
        if (!this->overlayAnimNames.Empty() && this->overlayAnimNames[0] == animName)
        {
            return;
        }
    }

    nArray<nString> anims;
    anims.PushBack(animName);
    nArray<float> weights;
    weights.PushBack(1.0f);

    this->SetOverlayAnimationMix(anims, weights, fadeIn, overrideDuration);
}

//------------------------------------------------------------------------------
/**
    Set a new overlay animation. This is usually an one-shot animation, like
    Bash, Jump, etc... After the overlay animation has finished, the
    current base animation will be re-activated.

    @param  animNames           new overlay animations
    @param  animWeights         new overlay animation weights
    @param  fadeIn              time to fade from current animation
    @param  overrideDuration    if != 0.0, override the animation's duration with this value
*/
void
CharEntity::SetOverlayAnimationMix(const nArray<nString>& animNames, const nArray<float>& animWeights, nTime fadeIn, nTime overrideDuration)
{
    n_assert(animNames.Size() >= 1);
    n_assert(animWeights.Size() >= 1);
    n_assert(this->nebCharacter);
    n_assert(0 != this->characterSet);

    nSkinAnimator* skinAnimator = this->nebCharacter->GetSkinAnimator();
    n_assert(0 != skinAnimator);

    const nString& mappedName = AnimTable::Instance()->Lookup(this->animMapping, animNames[0]).GetAnimName();
    int index = skinAnimator->GetClipIndexByName(mappedName);
    if (-1 != index)
    {
        nTime duration = skinAnimator->GetClipDuration(index);

        this->overlayAnimNames = animNames;
        this->overlayAnimWeights = animWeights;
        this->overlayAnimStarted = this->GetEntityTime();
        if (overrideDuration > 0.0)
        {
            this->overlayAnimDuration = overrideDuration;
        }
        else
        {
            nTime dur = 0.0;
            if (timeFactor > 0.0f)
            {
                dur = (duration - fadeIn) / this->timeFactor;
            }
            if (dur < 0.0)
            {
                dur = 0.0;
            }
            this->overlayAnimDuration = dur;
        }

        this->restartOverlayAnim = 1;
        this->overlayEndFadeIn = fadeIn;

        this->ActivateAnimations(this->overlayAnimNames, this->overlayAnimWeights, fadeIn);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::StopOverlayAnimation(nTime fadeIn)
{
    this->overlayAnimNames.Clear();
    this->overlayAnimWeights.Clear();
    this->restartOverlayAnim = 0;

    if (!this->baseAnimNames.Empty())
    {
        this->ActivateAnimations(this->baseAnimNames, this->baseAnimWeights, fadeIn);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::ActivateAnimations(const nArray<nString>& animNames, const nArray<float>& animWeights, nTime fadeIn)
{
    n_assert(animNames.Size() >= 1);
    n_assert(animWeights.Size() >= 1);
    n_assert(this->nebCharacter);
    n_assert(0 != this->characterSet);

    nSkinAnimator* skinAnimator = this->nebCharacter->GetSkinAnimator();
    n_assert(0 != skinAnimator);

    this->characterSet->ClearClips();
    this->characterSet->SetFadeInTime((float) fadeIn);

    for (int i = 0; i < animNames.Size(); i++)
    {
        if (animNames[i].IsValid())
        {
            const nString& mappedName = AnimTable::Instance()->Lookup(this->animMapping, animNames[i]).GetAnimName();
            this->characterSet->AddClip(mappedName, animWeights[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Update the render context variables.
    HACK NOTE: character entities are rotated 180 degrees for rendering.
*/
void
CharEntity::UpdateRenderContextVariables()
{
    Entity::UpdateRenderContextVariables();

    if (this->nebCharacter)
    {
        if (this->IsOverlayAnimationActive())
        {
            this->renderContext.GetVariable(this->animOffsetVarHandle)->SetFloat(0.0f);
        }
        else
        {
            this->renderContext.GetVariable(this->animOffsetVarHandle)->SetFloat((float) this->baseAnimOffset);
        }

        // communicate restart animation flag to Nebula
        this->renderContext.GetVariable(this->animRestartVarHandle)->SetInt(this->restartOverlayAnim);
        this->restartOverlayAnim = 0;

        // HACK: need to rotate characters by 180 degrees around Y
        matrix44 rot;
        rot.rotate_y(n_deg2rad(180.0f));
        rot.mult_simple(this->transform);
        this->renderContext.SetTransform(rot);
        this->shadowRenderContext.SetTransform(rot);
    }
}

//------------------------------------------------------------------------------
/**
    This method checks whether the current overlay animation is over
    and the base animation must be re-activated.
*/
void
CharEntity::OnRenderBefore()
{
    Entity::OnRenderBefore();

    if (this->nebCharacter)
    {
        if (this->IsOverlayAnimationActive())
        {
            // HACK: overlay animation needs to be stopped slightly before it's computed end time to avoid plopping
            static const nTime OverlayAnimStopBuffer = 0.3; // to assert overlay stopping before animation has finished
            nTime endTime = this->overlayAnimStarted + this->overlayAnimDuration - OverlayAnimStopBuffer;
            if (this->GetEntityTime() >= endTime)
            {
                this->StopOverlayAnimation(this->overlayEndFadeIn);

                if (this->activateNewBaseAnim && this->baseAnimNames.Size() > 0)
                {
                    n_assert(this->baseAnimWeights.Size() == this->baseAnimNames.Size());
                    this->activateNewBaseAnim = false;
                    ActivateAnimations(this->baseAnimNames, this->baseAnimWeights, this->baseAnimFadeIn);
                }
            }
        }

        // emit animation events
        n_assert(this->nebCharacter);
        float frameTime = float(Server::Instance()->GetFrameTime()) * this->timeFactor;
        float curTime  = float(this->GetEntityTime());
        float lastTime = curTime - frameTime;
        this->nebCharacter->EmitAnimEvents(lastTime, curTime);
    }

    if (this->character3Mode)
    {
        n_assert(this->character3NodePtr);
        this->SetLocalBox(this->character3NodePtr->GetLocalBox());
    }
}

//------------------------------------------------------------------------------
/**
    Returns a character joint index by its name. Returns -1 if a joint by
    that name doesn't exist in the character.
*/
int
CharEntity::GetJointIndexByName(const nString& name)
{
    n_assert(name.IsValid());
    if (this->nebCharacter)
    {
        return this->nebCharacter->GetSkeleton().GetJointIndexByName(name);
    }
    else
    {
        return -1;
    }
}

//------------------------------------------------------------------------------
/**
    This brings the character's skeleton uptodate. Make sure the
    entity's time and animation state weights in the render context are uptodate
    before calling this method, to avoid one-frame-latencies.
*/
void
CharEntity::EvaluateSkeleton()
{
    if (this->nebCharacter)
    {
        this->nebCharacter->EvaluateSkeleton((float) this->GetEntityTime());
    }
}

//------------------------------------------------------------------------------
/**
    Return pointer to joint or 0 if joint doesn't exist.
*/
nCharJoint*
CharEntity::GetJoint(int jointIndex) const
{
    if (this->nebCharacter)
    {
        return &(this->nebCharacter->GetSkeleton().GetJointAt(jointIndex));
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Returns a joint's current matrix in model space. Make sure to call
    EvaluateSkeleton() before!
*/
const matrix44&
CharEntity::GetJointMatrix(int jointIndex) const
{
    if (this->nebCharacter)
    {
        return this->nebCharacter->GetSkeleton().GetJointAt(jointIndex).GetMatrix();
    }
    else
    {
        static matrix44 identity;
        return identity;
    }
}

//------------------------------------------------------------------------------
/**
    Create animation event handler. This method will be called in CharEntity::OnActivate()
    to provide the animation event handler for this char entity.
    Override in subclass to provide a specific animation event handler.
*/
void
CharEntity::CreateAnimationEventHandler()
{
    this->animEventHandler = n_new(CharAnimEventHandler);
    this->animEventHandler->SetEntity(this);
}

//------------------------------------------------------------------------------
/**
    Cleanup the animation event handler. This method may be modified in subclasses
    if different cleanup is needed.
*/
void
CharEntity::CleanupAnimationEventHandler()
{
    n_assert(0 != this->animEventHandler);
    n_delete(this->animEventHandler);
    this->animEventHandler = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::LoadCharacter3Set(const nString& fileName)
{
    n_assert(fileName.IsValid());
    n_assert2(this->character3Mode && this->character3NodePtr, "CharEntity has Character 3 Resource");

    // assure resources are loaded
    if (!this->character3NodePtr->AreResourcesValid())
    {
        this->character3NodePtr->LoadResources();
    }

    // create new character 3 set
    nCharacter3Set* newCharacter3Set = n_new(nCharacter3Set);
    newCharacter3Set->Init(this->character3NodePtr);
    newCharacter3Set->LoadCharacterSetFromXML(this->character3NodePtr, fileName);
    this->characterSet = newCharacter3Set;

    // set new character set in render context
    nVariable& var = this->renderContext.GetLocalVar(this->characterSetHandle);
    nCharacter3Set* oldCharacterSet = (nCharacter3Set*) var.GetObj();
    oldCharacterSet->Release();
    var.SetObj(this->characterSet);

    this->character3SetFileName = fileName;
}

//------------------------------------------------------------------------------
/**
    Recursively Find instance of nClass, used to search for lights, skin
    animators, etc...
*/
nRoot*
CharEntity::FindFirstInstance(nRoot* node, nClass* classType)
{
    nRoot* resultNode = NULL;
    if (node == NULL)
    {
        resultNode = NULL;
    }
    else
    {
        if (node->IsInstanceOf(classType))
        {
            resultNode = node;
        }
        else
        {
            resultNode = FindFirstInstance(node->GetSucc(), classType);
            if (resultNode == NULL)
            {
                resultNode = FindFirstInstance(node->GetHead(), classType);
            }
        }
    }

    return resultNode;
}

} // namespace Graphics
