//------------------------------------------------------------------------------
//  properties/actorgraphicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/actorgraphicsproperty.h"
#include "attr/attributes.h"
#include "msg/gfxaddattachment.h"
#include "msg/gfxremattachment.h"
#include "msg/gfxsetanimation.h"
#include "msg/gfxaddskin.h"
#include "msg/gfxremskin.h"
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
        this->AddAttachment(jointName, rsrcName, offset);
    }
    else if (msg->CheckId(Message::GfxRemAttachment::Id))
    {
        Message::GfxRemAttachment* gfxRemAtt = (Message::GfxRemAttachment*) msg;
        this->RemAttachment(gfxRemAtt->GetJointName());
    }
    else if (msg->CheckId(Message::GfxSetAnimation::Id))
    {
        Message::GfxSetAnimation* gfxSetAnim = (Message::GfxSetAnimation*) msg;
        this->SetAnimation(gfxSetAnim->GetBaseAnimation(), gfxSetAnim->GetOverlayAnimation(), gfxSetAnim->GetBaseAnimTimeOffset());
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
    nString animSet = this->GetEntity()->GetString(Attr::AnimSet);
    if (animSet.IsValid())
    {
        ge->SetAnimationSet(animSet);
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
        ge->SetCharacterSet(nString("gfxlib:")+pathToCharSet+GetEntity()->GetString(Attr::CharacterSet)+".xml");
    };

    // set in parent class' graphics entity array
    this->graphicsEntities.Append(ge.get());
}

//------------------------------------------------------------------------------
/**
    Cleanup graphics entities
*/
void
ActorGraphicsProperty::CleanupGraphicsEntities()
{
    // clear our attachments
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    int i;
    int num = this->attachments.Size();
    for (i = 0; i < num; i++)
    {
        graphicsLevel->RemoveEntity(this->attachments[i].graphicsEntity);
    }
    this->attachments.Clear();
}

//------------------------------------------------------------------------------
/**
    Set base and/or overlay animation.
*/
void
ActorGraphicsProperty::SetAnimation(const nString& baseAnim, const nString& overlayAnim, float baseAnimTimeOffset)
{
    Graphics::CharEntity* gfxEntity = this->GetGraphicsEntity();
    if (baseAnim.IsValid())
    {
        gfxEntity->SetBaseAnimation(baseAnim, baseAnimTimeOffset);
    }
    if (overlayAnim.IsValid())
    {
        gfxEntity->SetOverlayAnimation(overlayAnim);
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
ActorGraphicsProperty::AddAttachment(const nString& jointName, const nString& gfxResName, const matrix44& offsetMatrix)
{
    n_assert(jointName.IsValid() && gfxResName.IsValid());

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
        // joint index is valid, create a new graphics entity
		Ptr<Graphics::Entity> ge = Graphics::Entity::Create();
        ge->SetResourceName(gfxResName);
        ge->SetVisible(false);

        // attach to graphics level
        Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
        n_assert(graphicsLevel);
        graphicsLevel->AttachEntity(ge);

        // create a new attachment structure
        Attachment newAttachment;
        newAttachment.jointIndex = jointIndex;
        newAttachment.offsetMatrix = offsetMatrix;
        newAttachment.graphicsEntity = ge;
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
        graphicsLevel->RemoveEntity(this->attachments[index].graphicsEntity);
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
        if (charEntity->IsNebulaCharacterInValidState())
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
        nCharacter3Set* char3Set = this->GetGraphicsEntity()->GetCharacter3Set();
        n_assert(char3Set);
        char3Set->SetSkinVisible(skinName, true);
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
        nCharacter3Set* char3Set = this->GetGraphicsEntity()->GetCharacter3Set();
        n_assert(char3Set);
        char3Set->SetSkinVisible(skinName, false);
    }
}

}; // namespace Properties
