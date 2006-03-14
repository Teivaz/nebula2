//------------------------------------------------------------------------------
//  properties/timeproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/timeproperty.h"
#include "managers/timemanager.h"
#include "game/entity.h"
#include "attr/attributes.h"

namespace Properties
{
ImplementRtti(Properties::TimeProperty, Game::Property);
ImplementFactory(Properties::TimeProperty);

using namespace Game;
using namespace Managers;

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
    GetEntity()->SetFloat(Attr::Time, 0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::OnActivate()
{
    Property::OnActivate();
    this->attachTime = TimeManager::Instance()->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::OnBeginFrame()
{
    nTime absTime = TimeManager::Instance()->GetTime();
    nTime relTime = absTime - this->attachTime;
    GetEntity()->SetFloat(Attr::Time, float(relTime));
}

}; // namespace Properties
