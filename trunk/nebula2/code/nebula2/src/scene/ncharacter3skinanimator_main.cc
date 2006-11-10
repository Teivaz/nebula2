//------------------------------------------------------------------------------
//  nCharacter3SkinAnimator_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ncharacter3skinanimator.h"
#include "scene/nskinshapenode.h"
#include "scene/nshadowskinshapenode.h"
#include "anim2/nanimation.h"
#include "anim2/nanimationserver.h"
#include "character/ncharacter2.h"
#include "scene/nrendercontext.h"
#include "variable/nvariableserver.h"
#include "kernel/ndirectory.h"
#include "character/ncharacter3set.h"
#include "resource/nresourceserver.h"

nNebulaClass(nCharacter3SkinAnimator, "nskinanimator");

//------------------------------------------------------------------------------
/**
*/
nCharacter3SkinAnimator::nCharacter3SkinAnimator() :
    currentVariation(-1)
{
}

//------------------------------------------------------------------------------
/**
*/
nCharacter3SkinAnimator::~nCharacter3SkinAnimator()
{
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter3SkinAnimator::Animate(nSceneNode* sceneNode, nRenderContext* renderContext)
{
    n_assert(sceneNode);
    n_assert(renderContext);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);


    const nVariable& charVar = renderContext->GetLocalVar(this->characterVarIndex);
    nCharacter2* curCharacter = (nCharacter2*) charVar.GetObj();
    n_assert(curCharacter);

    const nVariable& varCharacterSet = renderContext->GetLocalVar(this->characterSetIndex);
    nCharacter3Set* characterSet = (nCharacter3Set*) varCharacterSet.GetObj();
    n_assert(characterSet);

    this->currentVariation = characterSet->GetCurrentVariationIndex();

    // check if I am already uptodate for this frame
    uint curFrameId = renderContext->GetFrameId();
    if (curCharacter->GetLastEvaluationFrameId() != curFrameId)
    {
        curCharacter->SetLastEvaluationFrameId(curFrameId);

        // get the sample time from the render context
        nVariable* var = renderContext->GetVariable(this->channelVarHandle);
        n_assert2(0 != var, "nCharacter3SkinAnimator::Animate(): TimeChannel Variable in RenderContext.\n");
        const float curTime = var->GetFloat();

        // get the time offset from the render context
        var = renderContext->GetVariable(this->channelOffsetVarHandle);
        const float curOffset = 0 != var ? var->GetFloat() : 0.0f;

        // get character 2 set from render context and check if animation state needs to be updated
        if (characterSet->IsDirty())
        {
            nAnimStateInfo newState;
            int numClips = characterSet->GetNumClips();
            int i;

            float weightSum = 0.0f;
            for (i = 0; i < numClips; i++)
            {
                weightSum += characterSet->GetClipWeightAt(i);
            }

            // add clips
            if (weightSum > 0.0f)
            {
                newState.SetStateStarted(curTime);
                newState.SetFadeInTime(characterSet->GetFadeInTime());
                newState.BeginClips(numClips);
                for (i = 0; i < numClips; i++)
                {
                    int index = this->GetClipIndexByName(characterSet->GetClipNameAt(i));
                    if (-1 == index)
                    {
                        nRoot* parent = this->GetParent();
                        n_assert(parent);
                        n_error("nCharacter3SkinAnimator::Animate(): Requested clip \"%s\" does not exist on resource \"%s\".\n", characterSet->GetClipNameAt(i).Get(), parent->GetName());
                    }
                    newState.SetClip(i, this->GetClipAt(index), characterSet->GetClipWeightAt(i) / weightSum);
                }
                newState.EndClips();
            }

            curCharacter->SetActiveState(newState);
            characterSet->SetDirty(false);
        }

        // evaluate the current state of the character skeleton
        curCharacter->EvaluateSkeleton(curTime);

        // now apply the variation on the skeleton
        nCharSkeleton& skeleton = curCharacter->GetSkeleton();
        if (this->currentVariation != -1)
        {
            const nArray<nCharJoint>& varJoints = this->variationJoints[this->currentVariation];
            n_assert(varJoints.Size() == skeleton.GetNumJoints());
            int jointIndex;
            for (jointIndex = 0; jointIndex < varJoints.Size(); jointIndex++)
            {
                const vector3& varScale = varJoints[jointIndex].GetScale();
                skeleton.GetJointAt(jointIndex).SetVariationScale(varScale);
            }
            skeleton.Evaluate();
        }
        else
        {
            // no variation applied, reset variation scale to one
            int jointIndex;
            static const vector3 noScale(1.0f, 1.0f, 1.0f);
            for (jointIndex = 0; jointIndex < skeleton.GetNumJoints(); jointIndex++)
            {
                skeleton.GetJointAt(jointIndex).SetVariationScale(noScale);
            }
            skeleton.Evaluate();
        }
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
nArray<nCharJoint>
nCharacter3SkinAnimator::EvaluateVariation(nMemoryAnimation* variation)
{
    nArray<nCharJoint> result;

    nAnimStateInfo stateInfo;
    stateInfo.BeginClips(1);
    stateInfo.SetClip(0, this->clips[0], 1.0f);
    stateInfo.EndClips();

    this->character.SetAnimation(variation);
    this->character.SetActiveState(stateInfo);
    // evaluate the skeleton at time 30s, now the transformation should be complete
    this->character.EvaluateSkeleton(30.0f);

    int i;
    int numJoints = this->character.GetSkeleton().GetNumJoints();
    for (i = 0; i < numJoints; i++)
    {
        nCharJoint &joint = this->character.GetSkeleton().GetJointAt(i);
        result.Append(joint);
    }
    return result;
}


//------------------------------------------------------------------------------
/**
    -20-Apr-06  floh    cleaned up a bit
*/
bool
nCharacter3SkinAnimator::LoadResources()
{
    bool result = true;

    if (!this->AreResourcesValid())
    {
        // NOTE: don't call nSkinAnimator::LoadResources cause anim file is only dummy
        result &= nSceneNode::LoadResources();
        if (result)
        {
            nRoot* parent = this->GetParent();
            n_assert(parent);
            nString name = parent->GetName();
            this->animationNames = this->LoadAnimationsFromFolder(nString("anims:characters/")+name+"/animations",this->animationAnims);
            this->variationNames = this->LoadAnimationsFromFolder(nString("anims:characters/")+name+"/variations",this->variationAnims);

            // create a combined animation object, throw away original animations
            // so they don't use up memory
            nString rsrcName = name + nString("_c3anim");
            this->animations = (nCombinedAnimation*) nResourceServer::Instance()->NewResource("ncombinedanimation", rsrcName, nResource::Animation);
            this->animations->BeginAnims();
            int numAnimations = this->animationAnims.Size();
            int i;
            for (i = 0; i < numAnimations; i++)
            {
                this->animations->AddAnim(this->animationAnims[i]);
            }
            this->animations->EndAnims();
            for (i = 0; i < numAnimations; i++)
            {
                this->animationAnims[i]->Release();
                n_assert(!this->animationAnims[i].isvalid());
            }

            this->UnloadAnim();
            this->refAnim = this->animations;

            // create the clips
            this->BeginClips(numAnimations);
            for (i = 0; i < numAnimations; i++)
            {
                this->SetClip(i, i, this->animationNames[i]);
            }
            this->EndClips();

            // evaluate variations, throw away original variation animation, so they don't
            // use up memory
            for (i = 0; i < this->variationAnims.Size(); i++)
            {
                this->variationJoints.Append(this->EvaluateVariation(this->variationAnims[i]));
                this->variationAnims[i]->Release();
                n_assert(!this->variationAnims[i].isvalid());
            }

            /// at last set the current animation object on my character
            this->character.SetAnimation(this->animations);
        }
    }

    return result;
}


//------------------------------------------------------------------------------
/**
*/
void
nCharacter3SkinAnimator::UnloadResources()
{
    if (this->AreResourcesValid())
    {
        nSkinAnimator::UnloadResources();

        // NOTE Floh: this should already have happened
        // in LoadResources when variations have been
        // evaluated
        int i;
        for (i = 0; i < this->variationAnims.Size(); i++)
        {
            if (this->variationAnims[i].isvalid())
            {
                this->variationAnims[i]->Release();
                this->variationAnims[i].invalidate();
            }
        }

        // NOTE Floh: this should already have happened
        // in LoadResources when the source anims have been
        // combined
        for (i = 0; i < this->animationAnims.Size(); i++)
        {
            if (this->animationAnims[i].isvalid())
            {
                this->animationAnims[i]->Release();
                this->animationAnims[i].invalidate();
            }
        }

        if (this->animations.isvalid())
        {
            this->animations->Release();
        }
    }
}

//------------------------------------------------------------------------------
/**
    - 20-Apr-06 floh    cleaned up
*/
nArray<nString>
nCharacter3SkinAnimator::LoadAnimationsFromFolder(const nString& path, nArray<nRef<nMemoryAnimation> >& outAnimArray)
{
    nArray<nString> result;
    nArray<nString> files = nFileServer2::Instance()->ListFiles(path);
    int i;
    for (i = 0; i < files.Size(); i++)
    {
        if (files[i].CheckExtension("nax2") || files[i].CheckExtension("nanim2"))
        {
            n_printf("Loading animation %s...\n", files[i].Get());

            // load a memory animation
            nString entryName = files[i].ExtractFileName();
            entryName.StripExtension();
            result.Append(entryName);
            nMemoryAnimation* anim = (nMemoryAnimation*) this->refAnimServer->NewMemoryAnimation(0);
            anim->SetFilename(files[i]);
            if (!anim->Load())
            {
                n_error("nCharacter3SkinAnimator: Error loading anim file '%s'\n", files[i].Get());
                return result;
            }
            outAnimArray.Append(anim);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2Set*
nCharacter3SkinAnimator::CreateCharacterSet()
{
    return n_new (nCharacter3Set);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter3SkinAnimator::DeleteCharacterSet(nRenderContext* renderContext)
{
    n_assert(0 != renderContext);
    nVariable var = renderContext->GetLocalVar(this->characterSetIndex);
    nCharacter3Set* characterSet = (nCharacter3Set*) var.GetObj();
    n_assert(characterSet);
    characterSet->Release();
}
