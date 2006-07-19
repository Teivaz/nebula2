//------------------------------------------------------------------------------
//  properties/actorgraphicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/actorgraphicsproperty.h"
#include "msg/gfxaddattachment.h"
#include "msg/gfxremattachment.h"
#include "msg/gfxsetanimation.h"
#include "msg/gfxaddskin.h"
#include "msg/gfxremskin.h"
#include "msg/gfxsetcharacterset.h"
#include "foundation/factory.h"
#include "game/entity.h"
#include "graphics/server.h"
#include "graphics/level.h"
#include "graphics/resource.h"
#include "properties/actorphysicsproperty.h"
#include "managers/timemanager.h"

namespace Properties
{
ImplementRtti(Properties::ActorGraphicsProperty, Properties::GraphicsProperty);
ImplementFactory(Properties::ActorGraphicsProperty);

//------------------------------------------------------------------------------
/**
*/
ActorGraphicsProperty::~ActorGraphicsProperty()
{
    n_assert(this->attachments.Size() == 0);
}

//------------------------------------------------------------------------------
/**
*/
bool
ActorGraphicsProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::GfxAddAttachment::Id)) return true;
    if (msg->CheckId(Message::GfxRemAttachment::Id)) return true;
    if (msg->CheckId(Message::GfxSetAnimation::Id)) return true;
    if (msg->CheckId(Message::GfxAddSkin::Id)) return true;
    if (msg->CheckId(Message::GfxRemSkin::Id)) return true;
    if (msg->CheckId(Message::GfxSetCharacterSet::Id)) return true;
    if (msg->CheckId(Message::UpdateTransform::Id)) return true;
    return GraphicsProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
ActorGraphicsProperty::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::GfxAddAttachment::Id))
    {
        Message::GfxAddAttachment* gfxAddAtt = (Message::GfxAddAttachment*) msg;
        const nString& jointName = gfxAddAtt->GetJointName();
        const nString& rsrcName  = gfxAddAtt->GetResourceName();
        const matrix44& offset   = gfxAddAtt->GetOffsetMatrix();
        Graphics::Entity* gfxEntity = gfxAddAtt->GetGraphicsEntity();
        this->AddAttachment(jointName, rsrcName, offset, gfxEntity);
    }
    else if (msg->CheckId(Message::GfxRemAttachment::Id))
    {
        Message::GfxRemAttachment* gfxRemAtt = (Message::GfxRemAttachment*) msg;
        this->RemAttachment(gfxRemAtt->GetJointName());
    }
    else if (msg->CheckId(Message::GfxSetAnimation::Id))
    {
        this->SetAnimation((Message::GfxSetAnimation*) msg);
    }    
    else if (msg->CheckId(Message::GfxAddSkin::Id))
    {
        Message::GfxAddSkin* gfxAddSkin = (Message::GfxAddSkin*) msg;
        this->AddSkin(gfxAddSkin->GetSkinName());
    }
    else if (msg->CheckId(Message::GfxRemSkin::Id))
    {
        Message::GfxRemSkin* gfxRemSkin = (Message::GfxRemSkin*) msg;
        this->RemSkin(gfxRemSkin->GetSkinName());
    }
    else if (msg->CheckId(Message::GfxSetCharacterSet::Id))
    {
        Message::GfxSetCharacterSet* gfxSetCharacterSet = (Message::GfxSetCharacterSet*) msg;
        this->SetCharacterSet(gfxSetCharacterSet->GetCharacterSet());
    }
    else if (msg->CheckId(Message::UpdateTransform::Id))
    {
        Message::UpdateTransform* updTransform = (Message::UpdateTransform*) msg;
        this->graphicsEntities[0]->SetTransform(updTransform->GetMatrix());
    }
    else
    {
        GraphicsProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActorGraphicsProperty::SetupDefaultAttributes()
{
    GraphicsProperty::SetupDefaultAttributes();
    GetEntity()->SetString(Attr::AnimSet, "");
    GetEntity()->SetString(Attr::CharacterSet, "");
}

//------------------------------------------------------------------------------
/**
    This sets up a single character graphics entity. This method is called
    by OnActivate() of the parent class.
*/
void
ActorGraphicsProperty::SetupGraphicsEntities()
{
    n_assert(this->GetEntity()->HasAttr(Attr::Transform));
    
    // create and setup graphics property
	Ptr<Graphics::CharEntity> ge = Graphics::CharEntity::Create();
    ge->SetResourceName(this->GetEntity()->GetString(Attr::Graphics));
    ge->SetTransform(this->GetEntity()->GetMatrix44(Attr::Transform));
    nString animMapping = this->GetEntity()->GetString(Attr::AnimSet);
    if (animMapping.IsValid())
    {
        ge->SetAnimationMapping(animMapping);
    }
    else
    {
        n_error("ActorGraphicsProperty::SetupGraphicsEntity(): entity '%s' has empty AnimSet attribute!", 
            this->GetEntity()->GetString(Attr::Id).Get());
    }
    
    // attach graphics property to level
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    graphicsLevel->AttachEntity(ge);

    // TODO : setting to character3mode dirty, only if graphic path matches a character3
    nArray<nString> tokens;
    nString pathToCharSet = GetEntity()->GetString(Attr::Graphics);
    int numTokens = pathToCharSet.Tokenize("/",tokens);
    if((numTokens == 3) && (tokens[0] == "characters") && (tokens[2] == "skeleton"))
    {
        pathToCharSet = pathToCharSet.ExtractDirName() + "skinlists/";
        ge->LoadCharacter3Set(nString("gfxlib:")+pathToCharSet+GetEntity()->GetString(Attr::CharacterSet)+".xml");
    };

    // set in parent class' graphics entity array
    this->graphicsEntities.Append(ge.get());
}

//------------------------------------------------------------------------------
/**
*/
void
ActorGraphicsProperty::OnDeactivate()
{
    // clear our attachments
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    int i;
    int num = this->attachments.Size();
    for (i = 0; i < num; i++)
    {
        if (this->attachments[i].newCreated)
        {
            graphicsLevel->RemoveEntity(this->attachments[i].graphicsEntity);
        }
    }
    this->attachments.Clear();
    
    GraphicsProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    Set base and/or overlay animation.
*/
void
ActorGraphicsProperty::SetAnimation(Message::GfxSetAnimation* msg)
{
    n_assert(0 != msg);
    Graphics::CharEntity* gfxEntity = this->GetGraphicsEntity();
    if (msg->GetBaseAnimation().IsValid())
    {
        if (msg->GetBaseAnimation() != gfxEntity->GetBaseAnimation())
        {
            gfxEntity->SetBaseAnimation(msg->GetBaseAnimation(), msg->GetFadeInTime(), msg->GetBaseAnimTimeOffset());
        }
    }
    if (msg->GetOverlayAnimStop())
    {
        gfxEntity->StopOverlayAnimation(msg->GetFadeInTime());
    }
    if (msg->GetOverlayAnimation().IsValid())
    {
        gfxEntity->SetOverlayAnimation(msg->GetOverlayAnimation(), msg->GetFadeInTime(), msg->GetOverlayAnimDurationOverride());
    }
}

//------------------------------------------------------------------------------
/**
    Called before rendering happens. This processes pending messages.
*/
void
ActorGraphicsProperty::OnRender()
{
    this->HandlePendingMessages();
    GraphicsProperty::OnRender();
    this->UpdateAttachments();
}

//------------------------------------------------------------------------------
/**
    Find first attachment definition matching a given joint name.
*/
int
ActorGraphicsProperty::FindAttachment(const nString& jointName)
{
    n_assert(jointName.IsValid());

    // resolve joint name into joint index
    Graphics::CharEntity* charEntity = this->GetGraphicsEntity();
    int jointIndex = charEntity->GetJointIndexByName(jointName);
    if (-1 != jointIndex)
    {
        int num = this->attachments.Size();
        int i;
        for (i = 0; i < num; i++)
        {
            if (jointIndex == this->attachments[i].jointIndex)
            {
                return i;
            }
        }
    }
    // fallthrough: not found
    return -1;
}

//------------------------------------------------------------------------------
/**
    Add a graphics attachment to the entity. This will create a new
    graphics entity defined by the gfxRsrcName, and attaches it
    to a character joint.
*/
void
ActorGraphicsProperty::AddAttachment(const nString& jointName, const nString& gfxResName, const matrix44& offsetMatrix, Graphics::Entity* gfxEntity)
{
    n_assert(jointName.IsValid() && (gfxResName.IsValid() || gfxEntity));

    // first check if previous attachment is identical to the current,
    // do nothing in this case
    int attIndex = this->FindAttachment(jointName);
    if ((attIndex != -1) && this->attachments[attIndex].graphicsEntity.isvalid())
    {
        if (this->attachments[attIndex].graphicsEntity->GetResourceName() == gfxResName)
        {
            // identical attachment, do nothing
            return;
        }
    }

    // remove previous attachment
    this->RemAttachment(jointName);

    // resolve joint name into joint index
    Graphics::CharEntity* charEntity = this->GetGraphicsEntity();
    n_assert(charEntity && charEntity->IsA(Graphics::CharEntity::RTTI));
    int jointIndex = charEntity->GetJointIndexByName(jointName);
    if (-1 != jointIndex)
    {
        bool newCreated;
        Ptr<Graphics::Entity> ge;
        // if no gfxEntity is offered create a new one by resource name
        if (!gfxEntity)
        {
            // joint index is valid, create a new graphics entity
		    ge = Graphics::Entity::Create();
            ge->SetResourceName(gfxResName);
            ge->SetVisible(false);
            newCreated = true;

            // attach to graphics level
            Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
            n_assert(graphicsLevel);
            graphicsLevel->AttachEntity(ge);
        }
        else
        {
            ge = gfxEntity;
            newCreated = false;
        }

        // create a new attachment structure
        Attachment newAttachment;
        newAttachment.jointIndex = jointIndex;
        newAttachment.offsetMatrix = offsetMatrix;
        newAttachment.graphicsEntity = ge;
        newAttachment.newCreated = newCreated;
        this->attachments.Append(newAttachment);
    }
}

//------------------------------------------------------------------------------
/**
    Removes all existing attachment definitions from a joint.
*/
void
ActorGraphicsProperty::RemAttachment(const nString& jointName)
{
    n_assert(jointName.IsValid());
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    int index;
    while ((index = this->FindAttachment(jointName)) != -1)
    {
        if (this->attachments[index].newCreated)
        {
            graphicsLevel->RemoveEntity(this->attachments[index].graphicsEntity);
        }
        this->attachments.Erase(index);
    }
}

//------------------------------------------------------------------------------
/**
    Updates the transformations of all attachments.
*/
void
ActorGraphicsProperty::UpdateAttachments()
{
    if (this->attachments.Size() > 0)
    {
        Graphics::CharEntity* charEntity = this->GetGraphicsEntity();
        if (0 != charEntity)
        {
            // update Nebula character skeleton
            charEntity->EvaluateSkeleton();

            // get my entity's transformation
            const matrix44& worldMatrix = charEntity->GetTransform();

            // a correctional matrix which rotates the joint matrix by 180
            // degree around global Y to account for the fact the Nebula2
            // characters look along +Z, not -Z
            matrix44 rot180;
            rot180.rotate_y(n_deg2rad(180.0f));

            // for each attachment...
            int numAttachments = this->attachments.Size();
            int i;
            for (i = 0; i < numAttachments; i++)
            {
                const Attachment& curAttachment = this->attachments[i];

                // get attachment joint matrix...
                matrix44 jointMatrix = curAttachment.offsetMatrix;
                jointMatrix.mult_simple(charEntity->GetJointMatrix(curAttachment.jointIndex));

                // rotate the matrix by 180 degree (since Nebula2 characters look into -z)!!
                jointMatrix.mult_simple(rot180);
                jointMatrix.mult_simple(worldMatrix);

                // ...and update attachment graphics entity
                Graphics::Entity* ge = curAttachment.graphicsEntity;
                ge->SetTransform(jointMatrix);
                if (!ge->GetVisible())
                {
                    ge->SetVisible(true);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Makes the given skin visible if our graphics entity has a Character3.
*/
void
ActorGraphicsProperty::AddSkin(const nString& skinName)
{
    n_assert(skinName.IsValid());
    if (this->GetGraphicsEntity()->HasCharacter3Set())
    {
        nCharacter3Set* character3Set = (nCharacter3Set*) this->GetGraphicsEntity()->GetCharacterSet();
        n_assert(character3Set);
        character3Set->SetSkinVisible(skinName, true);
    }
}

//------------------------------------------------------------------------------
/**
    Makes the given skin invisible if our graphics entity has a Character3.
*/
void
ActorGraphicsProperty::RemSkin(const nString& skinName)
{
    n_assert(skinName.IsValid());
    if (this->GetGraphicsEntity()->HasCharacter3Set())
    {
        nCharacter3Set* character3Set = (nCharacter3Set*) this->GetGraphicsEntity()->GetCharacterSet();
        n_assert(character3Set);
        character3Set->SetSkinVisible(skinName, false);
    }
}

//------------------------------------------------------------------------------
/**
    Sets the character set of the graphics entity.
*/
void
ActorGraphicsProperty::SetCharacterSet(const nString& characterSetName)
{
    n_assert(characterSetName.IsValid());

    Graphics::CharEntity* charEntity = this->GetGraphicsEntity();
    n_assert(charEntity);

    // ignore message if not Character3 character
    if (!charEntity->HasCharacter3Set())
    {
        return;
    }

    // TODO: remove loaded skins/stop running animations?

    // build filename of character set
    // TODO : setting to character3mode dirty, only if graphic path matches a character3
    nArray<nString> tokens;
    nString pathToCharSet = this->GetEntity()->GetString(Attr::Graphics);
    int numTokens = pathToCharSet.Tokenize("/", tokens);
    if ((numTokens == 3) && (tokens[0] == "characters") && (tokens[2] == "skeleton"))
    {
        pathToCharSet = pathToCharSet.ExtractDirName() + "skinlists/";
        
        nString filename;
        filename.Format("gfxlib:%s%s.xml", 
            pathToCharSet.Get(), 
            characterSetName.Get());

        charEntity->LoadCharacter3Set(filename);
    }
    else
    {
        // invalid path to character set, I'm not sure if this is allowed to happen!?
        n_error("invalid path to character set: %s", pathToCharSet.Get());
    }
}

}; // namespace Properties
