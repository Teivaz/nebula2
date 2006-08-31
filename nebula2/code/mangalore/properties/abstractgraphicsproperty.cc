//------------------------------------------------------------------------------
//  properties/abstractgraphicsproperty.h
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/abstractgraphicsproperty.h"
#include "attr/attributes.h"
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
    Get the default graphics resource, which is Attr::Graphics.
    subclasses may override this to provide a self managed resource.
*/
nString
AbstractGraphicsProperty::GetGraphicsResource()
{
    return GetEntity()->GetString(Attr::Graphics);
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
