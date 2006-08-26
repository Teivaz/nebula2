//------------------------------------------------------------------------------
//  nCharacter3SkinAnimator_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ncharacter3skinanimator.h"
#include "scene/nskinshapenode.h"
#include "scene/nshadowskinshapenode.h"
#include "anim2/nanimation.h"
#include "anim2/nanimationserver.h"
#include "anim2/nanimstatearray.h"
#include "character/ncharacter2.h"
#include "scene/nrendercontext.h"
#include "variable/nvariableserver.h"
#include "kernel/ndirectory.h"
#include "character/ncharacter3set.h"

nNebulaScriptClass(nCharacter3SkinAnimator, "nskinanimator");

//------------------------------------------------------------------------------
/**
*/
nCharacter3SkinAnimator::nCharacter3SkinAnimator():
    animations(0),
    variations(0),
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
nCharacter3SkinAnimator::SetCharacterSetIndexHandle(int handle)
{
    this->characterSetIndex = handle;
};

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

    this->currentVariation = -1;
    nVariable& varCharacter3Set = renderContext->GetLocalVar(this->characterSetIndex);
    nCharacter3Set* charSet = (nCharacter3Set*) varCharacter3Set.GetObj();
    if((charSet) && (charSet->IsValid()))
    {
        this->currentVariation = charSet->GetCurrentVariationIndex();
    };

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

        // now apply the variation on the skeleton
        if(this->currentVariation != -1)
        {
            // just multiplying the scaling here

            // make a copy of the skeleton
            nCharSkeleton &skeleton = curCharacter->GetVariationSkeleton();
            skeleton = curCharacter->GetSkeleton();

            nArray<nCharJoint> *jointList = &this->variationJoints[this->currentVariation];
            int i;
            for(i = 0; i < jointList->Size(); i++)
            {
                nCharJoint &joint = skeleton.GetJointAt(i);
                vector3 scale = joint.GetScale();
                vector3 scale2 = jointList->At(i).GetScale();
                scale.x *= scale2.x;
                scale.y *= scale2.y;
                scale.z *= scale2.z;
                joint.SetScale(scale);
            };
            skeleton.Evaluate();
        };

    }

    // update the source node with the new char skeleton state
    if (sceneNode->IsA(this->skinShapeNodeClass))
    {
        nSkinShapeNode* skinShapeNode = (nSkinShapeNode*) sceneNode;
        if((!skinShapeNode->GetChar3VariationFlag()) && (this->currentVariation != -1) )
        {
            skinShapeNode->SetCharSkeleton(&curCharacter->GetVariationSkeleton());
        }
        else
        {
            skinShapeNode->SetCharSkeleton(&curCharacter->GetSkeleton());
        };
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
nCharacter3SkinAnimator::EvaluteVariation(nRef<nMemoryAnimation> variation)
{
    nArray<nCharJoint> result;

    // create a temporary context, this is needed to evaluate the skeleton
    nVariableContext    context;

    nVariable           var(this->GetStateAt(0).GetClipAt(0).GetWeightChannelHandle(),1.0f);
    context.AddVariable(var);

    this->character.SetAnimation(variation);
    this->character.SetActiveState(0,0,0);
    // evaluate the skeleton at time 30s, now the transformation should be complete
    this->character.EvaluateSkeleton(30.0f,&context);

    int i;
    int numJoints = this->character.GetSkeleton().GetNumJoints();
    for(i = 0; i < numJoints; i++)
    {
        nCharJoint &joint = this->character.GetSkeleton().GetJointAt(i);
        result.Append(joint);
    };
    return result;
};


//------------------------------------------------------------------------------
/**
*/
bool
nCharacter3SkinAnimator::LoadResources()
{
    bool result = true;

    if(!this->AreResourcesValid())
    {
        result &= nSkinAnimator::LoadResources();
        if(result)
        {
            nRoot *parent = this->GetParent();
            n_assert(parent);
            nString name = parent->GetName();
            this->animationNames = this->LoadAnimationsFromFolder(nString("anims:characters/")+name+"/animations",this->animationAnims);
            this->variationNames = this->LoadAnimationsFromFolder(nString("anims:characters/")+name+"/variations",this->variationAnims);

            // evaluate variations
            int i;
            for(i = 0; i < this->variationAnims.Size(); i++)
            {
                this->variationJoints.Append(this->EvaluteVariation(this->variationAnims[i]));
            };

            // reset the character
            this->character.ResetCurrentState();

            // combine animations
            this->animations = (nCombinedAnimation*) nKernelServer::Instance()->New("ncombinedanimation","character3animations");
            n_assert(this->animations);
            this->animations->BeginAnims();
            int numAnimations = this->animationAnims.Size();
            for( i = 0; i < numAnimations; i++)
            {
                this->animations->AddAnim(this->animationAnims[i]);
            };

            this->animations->EndAnims();

            this->character.SetAnimation(this->animations);

            // create the states
            this->BeginStates(numAnimations);
            for( i = 0; i < numAnimations; i++)
            {
                this->SetState(i,i, 0.3f);
                this->SetStateName(i, this->animationNames[i]);
                this->BeginClips(i, 1);
                this->SetClip(i, 0, "one");
                this->EndClips(i);
            };
            this->EndStates();
        };
    };

    return result;
};


//------------------------------------------------------------------------------
/**
*/
void
nCharacter3SkinAnimator::UnloadResources()
{
    if(this->AreResourcesValid())
    {
        nSkinAnimator::UnloadResources();
        int i;
        for(i = 0; i < this->variationAnims.Size(); i++)
        {
            if (this->variationAnims[i].isvalid())
            {
                this->variationAnims[i]->Release();
                this->variationAnims[i].invalidate();
            }
        };
        for(i = 0; i < this->animationAnims.Size(); i++)
        {
            if (this->animationAnims[i].isvalid())
            {
                this->animationAnims[i]->Release();
                this->animationAnims[i].invalidate();
            }
        };
        if(this->animations)
        {
            this->animations->Release();
        };
        if(this->variations)
        {
            this->variations->Release();
        };
    };
};

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3SkinAnimator::LoadAnimationsFromFolder(nString path,nArray<nRef<nMemoryAnimation> > &animArray)
{
    nArray<nString> result;
    nDirectory* dir = nFileServer2::Instance()->NewDirectoryObject();
    if (dir->Open(path.Get()))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (dir->GetEntryType() == nDirectory::FILE)
            {
                nString curPath = dir->GetEntryName();
                if(curPath.CheckExtension("nax2") || curPath.CheckExtension("nanim2"))
                {
                    n_printf("Loading animation %s ... ",curPath.Get());
                    // load new object

                    nString entryName = curPath.ExtractFileName();
                    entryName.StripExtension();
                    result.Append(entryName);

                    nMemoryAnimation* anim = (nMemoryAnimation*) this->refAnimServer->NewMemoryAnimation(entryName);
                    n_assert(anim);

                    anim->SetFilename(curPath);
                    if (!anim->Load())
                    {
                        n_printf("nCharacter3SkinAnimator: Error loading anim file '%s'\n", curPath.Get());
                        anim->Release();
                        n_assert(false);// just break
                        return result;
                    }
                    animArray.Append(anim);
                    n_printf("Done!\n");
                };
            };
        } while (dir->SetToNextEntry());
        dir->Close();
    };
    delete dir;
    return result;
};

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3SkinAnimator::GetNamesOfLoadedAnimations()
{
    return this->animationNames;
};

//------------------------------------------------------------------------------
/**
*/
nArray<nString>
nCharacter3SkinAnimator::GetNamesOfLoadedVariations()
{
    return this->variationNames;
};
