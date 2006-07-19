//------------------------------------------------------------------------------
//  game/attrset.h
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/attrset.h"

namespace Game
{
ImplementRtti(Game::AttrSet, Foundation::RefCounted);
ImplementFactory(Game::AttrSet);

//------------------------------------------------------------------------------
/**
*/
void
AttrSet::SetupEntity(Entity* entity)
{
    // empty
}

};
