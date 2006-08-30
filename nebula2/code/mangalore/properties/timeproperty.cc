//------------------------------------------------------------------------------
//  properties/timeproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/timeproperty.h"
#include "game/time/gametimesource.h"
#include "game/entity.h"
#include "attr/attributes.h"

namespace Properties
{
ImplementRtti(Properties::TimeProperty, Game::Property);
ImplementFactory(Properties::TimeProperty);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
TimeProperty::TimeProperty() :
    attachTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TimeProperty::~TimeProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Makes only sense for live entities.
*/
int
TimeProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    this->GetEntity()->SetFloat(Attr::Time, 0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::OnActivate()
{
    Property::OnActivate();
    this->attachTime = GameTimeSource::Instance()->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::OnBeginFrame()
{
    nTime absTime = GameTimeSource::Instance()->GetTime();
    nTime relTime = absTime - this->attachTime;
    this->GetEntity()->SetFloat(Attr::Time, float(relTime));
}

} // namespace Properties
