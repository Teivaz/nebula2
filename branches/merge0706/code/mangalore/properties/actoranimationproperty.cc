//------------------------------------------------------------------------------
//  properties/actoranimationproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/actoranimationproperty.h"
#include "game/entity.h"
#include "msg/gfxsetanimation.h"
#include "msg/movedirection.h"
#include "msg/movesetvelocity.h"
#include "msg/movestop.h"
#include "msg/moveturn.h"

namespace Properties
{
ImplementRtti(Properties::ActorAnimationProperty, Game::Property);
ImplementFactory(Properties::ActorAnimationProperty);

//------------------------------------------------------------------------------
/** 
    Animation is only active for live entities.
*/
int
ActorAnimationProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
bool
ActorAnimationProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::MoveDirection::Id)) return true;
    if (msg->CheckId(Message::MoveSetVelocity::Id)) return true;
    if (msg->CheckId(Message::MoveStop::Id)) return true;
    if (msg->CheckId(Message::MoveTurn::Id)) return true;
    return Game::Property::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
ActorAnimationProperty::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::MoveStop::Id))
    {
        this->RequestAnimation("Idle", "", 0.0f);
    }
    else if (msg->CheckId(Message::MoveDirection::Id) ||
             msg->CheckId(Message::MoveSetVelocity::Id))
    {
        this->RequestAnimation("Walk", "", 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActorAnimationProperty::OnActivate()
{
    Game::Property::OnActivate();
    
    // by default go to idle animation
    this->RequestAnimation("Idle", "", 0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
ActorAnimationProperty::RequestAnimation(const nString& baseAnimation, const nString& overlayAnimation, float baseAnimTimeOffset) const
{
    Ptr<Message::GfxSetAnimation> setAnimation = Message::GfxSetAnimation::Create();
    setAnimation->SetBaseAnimation(baseAnimation);
    setAnimation->SetOverlayAnimation(overlayAnimation);
    setAnimation->SetBaseAnimTimeOffset(baseAnimTimeOffset);

    this->GetEntity()->SendSync(setAnimation);
}

} // namespace Properties
