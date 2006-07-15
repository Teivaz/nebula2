//------------------------------------------------------------------------------
//  graphics/charentity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/charentity.h"
#include "variable/nvariableserver.h"
#include "character/ncharacter2.h"
#include "graphics/server.h"
#include "scene/ncharacter3node.h"
#include "kernel/nfileserver2.h"

namespace Graphics
{
ImplementRtti(Graphics::CharEntity, Graphics::Entity);
ImplementFactory(Graphics::CharEntity);

//------------------------------------------------------------------------------
/**
*/
CharEntity::CharEntity() :
    curBaseAnimIndex(0),
    baseAnimStarted(0.0),
    baseAnimOffset(0.0),
    curOverlayAnimIndex(-1),
    overlayAnimStarted(0.0),
    overlayAnimDuration(0.0),
    baseAnimDuration(0.0),
    nebCharacter(0),
    animEventHandler(0),
    restartOverlayAnim(0),
    character3Mode(false),
    char3VarHandle(nVariable::InvalidHandle),
    char3NodePtr(0)
{
    // initialize Nebula variable handles
    nVariableServer* varServer  = nVariableServer::Instance();
    this->animStateVarHandle    = varServer->GetVariableHandleByName("chnCharState");
    this->animRestartVarHandle  = varServer->GetVariableHandleByName("chnRestartAnim");
    this->animOffsetVarHandle   = varServer->GetVariableHandleByName("timeOffset");
    this->charPointerHandle     = varServer->GetVariableHandleByName("charPointer");
    this->renderContext.AddVariable(nVariable(this->animStateVarHandle, 0));
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
    nVariable* var = this->renderContext.FindLocalVar(this->charPointerHandle);
    if (var)
    {
        this->nebCharacter = (nCharacter2*) var->GetObj();
        n_assert(this->nebCharacter);
        this->nebCharacter->SetAnimEventHandler(this->animEventHandler);
    }
    this->animEventHandler->SetEntity(this);
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
        return this->nebCharacter->GetAnimStateArray()->GetNumStates();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Set base animation by index.
*/
void
CharEntity::SetBaseAnimationByIndex(int i, float timeOffset)
{
    if (this->nebCharacter)
    {
        n_assert((i >= 0) && (i < this->GetNumAnimations()));
        this->curBaseAnimIndex = i;
        this->baseAnimStarted = this->GetEntityTime();
        nTime dur = (this->nebCharacter->GetStateDuration(i) - this->nebCharacter->GetStateFadeInTime(i)) / this->timeFactor;
        if (dur <= 0.0)
        {
            dur = 0.0;
        }
        this->baseAnimDuration = dur;
        this->baseAnimOffset = timeOffset;
        if(this->character3Mode)
        {
            this->character3Set.SetCurrentAnimation(i);
        };
    }
}

//------------------------------------------------------------------------------
/**
    Set a new base animation. This is usually a looping animation, like
    Idle, Walking, Running, etc...

    @param  animName        new base animation
    @param  timeOffset      flag defines if played from start or from random offset
*/
void
CharEntity::SetBaseAnimation(const nString& animName, float timeOffset)
{
    n_assert(animName.IsValid());
    this->curBaseAnimName.Clear();

    // convert name to index
    nCharacter2* chr = this->GetCharacterPointer();
    if (chr)
    {
        n_assert(chr);

        const nString& mappedName = AnimTable::Instance()->Lookup(this->GetAnimationSet(), animName);

        if(this->character3Mode)
        {
            int animIndex = this->character3Set.ConvertAnimationNameToIndex(mappedName);
            this->SetBaseAnimationByIndex(animIndex, timeOffset);
            this->curBaseAnimName = animName;
        }
        else
        {
            int animIndex = chr->FindStateIndexByName(mappedName.Get());
            if (animIndex != -1)
            {
                if (animIndex != this->curBaseAnimIndex)
                {
                    this->SetBaseAnimationByIndex(animIndex, timeOffset);
                    this->curBaseAnimName = animName;
                }
                else
                {
                    //n_printf("CharEntity::SetBaseAnimation(): redundant anim '%s'\n", animName);
                }
            }
            else
            {
                //n_printf("CharEntity::SetBaseAnimation(): invalid anim '%s'\n", animName);
            }
        };
    }
}

//------------------------------------------------------------------------------
/**
    Set a new overlay animation. This is usually a oneshot animation, like
    Bash, Jump, etc... After the overlay animation has finished, the
    current base animation will be re-activated.

    @param  animName    new overlay animation
*/
void
CharEntity::SetOverlayAnimation(const nString& animName)
{
    n_assert(animName.IsValid());
    this->curOverlayAnimName.Clear();

    // convert name to index, note: overlay animation
    // are not checked for redundancy
    nCharacter2* chr = this->GetCharacterPointer();
    if (chr)
    {
        const nString& mappedName = AnimTable::Instance()->Lookup(this->GetAnimationSet(), animName);
        int animIndex = chr->FindStateIndexByName(mappedName.Get());
        if (animIndex != -1)
        {
            this->SetOverlayAnimationByIndex(animIndex);
            this->curOverlayAnimName = animName;
        }
        else
        {
            //n_printf("CharEntity::SetOverlayAnimation(): invalid anim '%s'\n", animName);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set overlay animation by index.
*/
void
CharEntity::SetOverlayAnimationByIndex(int i)
{
    n_assert((i >= 0) && (i < this->GetNumAnimations()));

    this->curOverlayAnimIndex = i;
    this->overlayAnimStarted = this->GetEntityTime();

    // get duration of overlay animation from Nebula2 character
    nCharacter2* chr = this->GetCharacterPointer();
    if (chr)
    {
        nTime dur = (chr->GetStateDuration(i) - chr->GetStateFadeInTime(i)) / this->timeFactor;
        if (dur <= 0.0)
        {
            dur = 0.0;
        }
        this->overlayAnimDuration = dur;
        this->restartOverlayAnim = 1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::StopOverlayAnimation()
{
    this->curOverlayAnimIndex = -1;
    this->curOverlayAnimName.Clear();
    this->restartOverlayAnim = 0;
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
        if (-1 != this->curOverlayAnimIndex)
        {
            this->renderContext.GetVariable(this->animStateVarHandle)->SetInt(this->curOverlayAnimIndex);
            this->renderContext.GetVariable(this->animOffsetVarHandle)->SetFloat(0.0f);
        }
        else
        {
            n_assert(-1 != this->curBaseAnimIndex);
            this->renderContext.GetVariable(this->animStateVarHandle)->SetInt(this->curBaseAnimIndex);
            this->renderContext.GetVariable(this->animOffsetVarHandle)->SetFloat((float)this->baseAnimOffset);
        }
        this->renderContext.GetVariable(this->animRestartVarHandle)->SetInt(this->restartOverlayAnim);
        matrix44 rot;
        rot.rotate_y(n_deg2rad(180.0f));
        rot.mult_simple(this->transform);
        this->renderContext.SetTransform(rot);
        this->shadowRenderContext.SetTransform(rot);
        this->restartOverlayAnim = 0;
    }

    if(this->character3Mode)
    {
        nVariable& var = this->renderContext.GetLocalVar(this->char3VarHandle);
        var.SetObj(&this->character3Set);
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
        if (-1 != this->curOverlayAnimIndex)
        {
            nTime endTime = this->overlayAnimStarted + this->overlayAnimDuration;
            if (this->GetEntityTime() >= endTime)
            {
                this->curOverlayAnimIndex = -1;
                this->curOverlayAnimName.Clear();
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
        n_assert(this->char3NodePtr);
        this->SetLocalBox(this->char3NodePtr->GetLocalBox());
    }
}

//------------------------------------------------------------------------------
/**
    DEBUG : this is just for the MILESTONE, check actorgraphicsproperty.cc
*/
void
CharEntity::LoadNextSkinListInDirectory()
{
    nString path = this->char3SetFileName.ExtractDirName();
    nString curFile = this->char3SetFileName.ExtractFileName();
    int sel = 0;
    nArray<nString> filesPure = nFileServer2::Instance()->ListFiles(path);
    nArray<nString> files;
    int i;
    for( i = 0; i < filesPure.Size(); i++)
    {
        if((filesPure[i].GetExtension() == nString("xml")) && (filesPure[i].ExtractFileName() != nString("_auto_default_.xml")))
        {
            files.Append(filesPure[i]);
        };
    };

    if(files.Size() > 0)
    {
        for(i = 0; i < files.Size(); i++)
        {
            if(files[i].ExtractFileName() == curFile)
            {
                sel = i+1;
            };
        };
        if(sel >= files.Size())
        {
            sel = 0;
        };
        this->char3SetFileName = path + files[sel].ExtractFileName();


        nTransformNode* charParentNode = this->GetResource().GetNode();
        nCharacter3Node* char3 = (nCharacter3Node*)charParentNode->GetHead();
        if(!char3->AreResourcesValid())
        {
            char3->LoadResources();
        };
        this->character3Set.LoadCharacterSetFromXML(char3,this->char3SetFileName);
    };
};

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
    entity's time and animation state weights in the rendercontext are uptodate
    before calling this method, to avoid one-frame-latencies.
*/
void
CharEntity::EvaluateSkeleton()
{
    if (this->nebCharacter)
    {
        this->nebCharacter->EvaluateSkeleton(float(this->GetEntityTime()), &this->renderContext);
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
        return &this->nebCharacter->GetSkeleton().GetJointAt(jointIndex);
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
        return matrix44::identity;
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the Nebula2's character object is currently in a valid
    state. This is only the case after the object has been rendered once.
*/
bool
CharEntity::IsNebulaCharacterInValidState() const
{
    if (this->nebCharacter)
    {
        return (this->nebCharacter->GetActiveState() != -1);
    }
    else
    {
        return false;
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
    set the characterset
*/
void
CharEntity::SetCharacterSet(nString fileName)
{
    // skip, if filename is empty
    if (!fileName.IsValid())
    {
        return;
    }

    if (!this->character3Mode)
    {
        this->character3Mode = true;
        this->char3SetFileName = fileName;
        this->char3NodePtr = 0;

        // /res/gfx/characters/skeleton/zweibeiner
        // now the basic character3 node is loaded, but needs to be initialized
        // first we need to find the character3 node
        nClass* nCharacter3NodeClass = nKernelServer::Instance()->FindClass("ncharacter3node");
        nTransformNode* charParentNode = this->GetResource().GetNode();

        // find the first ncharacter3node object under the parent node
        nRoot* charNode;
        for (charNode = charParentNode->GetHead();
             charNode;
             charNode = charNode->GetSucc())
        {
            if (charNode->IsA(nCharacter3NodeClass))
            {
                nCharacter3Node* char3 = (nCharacter3Node*) charNode;
                if (!char3->AreResourcesValid())
                {
                    char3->LoadResources();
                }
                this->char3NodePtr = char3;

                this->character3Set.Init(char3);
                this->character3Set.LoadCharacterSetFromXML(char3,fileName);

                this->char3VarHandle = char3->GetRenderContextCharacterSetIndex();
                nVariable& var = this->renderContext.GetLocalVar(this->char3VarHandle);
                var.SetObj(&this->character3Set);

                break;
            }
        }
        n_assert(this->char3NodePtr);
    }
}

} // namespace Graphics
