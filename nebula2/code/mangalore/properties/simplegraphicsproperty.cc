//------------------------------------------------------------------------------
//  properties/simplegraphicsproperty.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/simplegraphicsproperty.h"
#include "graphics/server.h"
#include "graphics/level.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "msg/updatetransform.h"

namespace Properties
{
ImplementRtti(Properties::SimpleGraphicsProperty, Properties::AbstractGraphicsProperty);
ImplementFactory(Properties::SimpleGraphicsProperty);

//------------------------------------------------------------------------------
/**
*/
SimpleGraphicsProperty::SimpleGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SimpleGraphicsProperty::~SimpleGraphicsProperty()
{
    n_assert(this->graphicsEntity == 0);
}

//------------------------------------------------------------------------------
/**
    Get a reference to the array of graphics entities
*/
Graphics::Entity*
SimpleGraphicsProperty::GetGraphicsEntity() const
{
    n_assert(this->graphicsEntity != 0);
    return this->graphicsEntity;
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleGraphicsProperty::SetupDefaultAttributes()
{
    AbstractGraphicsProperty::SetupDefaultAttributes();
    GetEntity()->SetString(Attr::Graphics, "");
}

//------------------------------------------------------------------------------
/**
    Attach the property to a game entity. This will create and setup
    the required graphics entities.
*/
void
SimpleGraphicsProperty::OnActivate()
{
    AbstractGraphicsProperty::OnActivate();

    this->SetupGraphicsEntity();
}

//------------------------------------------------------------------------------
/**
    Remove the property from its game entity. This will release the
    graphics entities owned by the property.
*/
void
SimpleGraphicsProperty::OnDeactivate()
{
    this->CleanupGraphicsEntity();

    // up to parent class
    AbstractGraphicsProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleGraphicsProperty::SetupGraphicsEntity()
{
    n_assert(GetEntity()->HasAttr(Attr::Transform));
    n_assert(this->graphicsEntity == 0);

    // create graphics entity
    this->graphicsEntity = Graphics::Entity::Create();
    this->graphicsEntity->SetUserData(GetEntity()->GetUniqueId());
    this->graphicsEntity->SetResourceName(GetEntity()->GetString(Attr::Graphics));
    this->graphicsEntity->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));

    // attach to level
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    graphicsLevel->AttachEntity(this->graphicsEntity);
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleGraphicsProperty::CleanupGraphicsEntity()
{
    if (this->graphicsEntity != 0)
    {
        Graphics::Level* gfxLevel = Graphics::Server::Instance()->GetLevel();
        n_assert(gfxLevel);

        // release graphics entity
        if (this->graphicsEntity->GetCell())
        {
            gfxLevel->RemoveEntity(this->graphicsEntity);
        }
        this->graphicsEntity = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
SimpleGraphicsProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::UpdateTransform::Id)) return true;
    return AbstractGraphicsProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleGraphicsProperty::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::UpdateTransform::Id))
    {
        // update the graphics entity transform
        if (this->graphicsEntity != 0)
        {
            Message::UpdateTransform* updateTransform = (Message::UpdateTransform*) msg;
            this->graphicsEntity->SetTransform(updateTransform->GetMatrix());
        }
    }
    else
    {
        AbstractGraphicsProperty::HandleMessage(msg);
    }
}

} // namespace Properties
