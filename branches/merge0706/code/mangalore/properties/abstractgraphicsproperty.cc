//------------------------------------------------------------------------------
//  properties/abstractgraphicsproperty.h
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/abstractgraphicsproperty.h"
#include "game/entity.h"

namespace Properties
{
ImplementRtti(Properties::AbstractGraphicsProperty, Game::Property);
ImplementFactory(Properties::AbstractGraphicsProperty);

//------------------------------------------------------------------------------
/** 
*/
AbstractGraphicsProperty::AbstractGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
*/
AbstractGraphicsProperty::~AbstractGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
    Makes only sense for live entities.
*/
int
AbstractGraphicsProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

} // namespace Properties
