//------------------------------------------------------------------------------
//  properties/transformableproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/transformableproperty.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "msg/settransform.h"

namespace Properties
{
ImplementRtti(Properties::TransformableProperty, Game::Property);
ImplementFactory(Properties::TransformableProperty);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
TransformableProperty::TransformableProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TransformableProperty::~TransformableProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TransformableProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    GetEntity()->SetMatrix44(Attr::Transform, matrix44::identity);
}

//------------------------------------------------------------------------------
/**
*/
bool
TransformableProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg != 0);
    if (msg->CheckId(Message::UpdateTransform::Id)) return true;
    if (msg->CheckId(Message::SetTransform::Id)) return true;
    return Game::Property::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
TransformableProperty::HandleMessage(Message::Msg* msg)
{
    n_assert(msg != 0);

    if (msg->CheckId(Message::UpdateTransform::Id))
    {
        // update the transformation of the game entity
        Message::UpdateTransform* updateTransform = (Message::UpdateTransform*) msg;
        this->GetEntity()->SetMatrix44(Attr::Transform, updateTransform->GetMatrix());
    }
    else if (msg->CheckId(Message::SetTransform::Id))
    {
        // create a update transform msg, to update the transformation
        Message::SetTransform* setTransform = (Message::SetTransform*) msg;
        Ptr<Message::UpdateTransform> updateTransform = Message::UpdateTransform::Create();
        updateTransform->SetMatrix(setTransform->GetMatrix());
        this->GetEntity()->SendSync(updateTransform);
    }
    else
    {
        Game::Property::HandleMessage(msg);
    }
}

} // namespace Properties
