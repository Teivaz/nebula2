#ifndef LOADER_ENTITYLOADERBASE_H
#define LOADER_ENTITYLOADERBASE_H
//------------------------------------------------------------------------------
/**
    @class Loader::EntityLoaderBase

    Abstract loader helper for game entities.

    (C) 2006 RadonLabs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Loader
{
class EntityLoaderBase : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// constructor
    EntityLoaderBase();
    /// destructor
    ~EntityLoaderBase();
    /// load entity objects into the level
    virtual bool Load(const nString& levelName);
};

} // namespace Loader
//------------------------------------------------------------------------------
#endif
