#ifndef GAME_ATTRSET_H
#define GAME_ATTRSET_H
//------------------------------------------------------------------------------
/**
    Attribute sets are objects that add attributes to entities and
    initialize them to their default values.

    (C) 2005 Radon Labs GmbH
*/
#include "attr/attributeid.h"
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Game
{
class Entity;

class AttrSet : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// Add attributes to entity.
    virtual void SetupEntity(Entity* entity) = 0;
    
};

}; // namespace Game
//------------------------------------------------------------------------------
#endif