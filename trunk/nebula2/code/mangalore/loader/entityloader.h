#ifndef LOADER_ENTITYLOADER_H
#define LOADER_ENTITYLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::EntityLoader
    
    Loader helper for universal game entities. The properties which are
    attached to the entity are described in blueprints.xml, the attributes
    to attach come from the world database.
    
    (C) 2005 Radon Labs GmbH
*/
#include "util/nstring.h"
#include "loader/entityloaderbase.h"

//------------------------------------------------------------------------------
namespace Loader
{

class EntityLoader : public EntityLoaderBase
{
    DeclareRtti;
    DeclareFactory(EntityLoader);
public:
    /// constructor
    EntityLoader();
    /// destructor
    ~EntityLoader();
    /// load entity objects into the level
    virtual bool Load(const nString& levelName);
};
RegisterFactory(EntityLoader);

}; // namespace Loader
//------------------------------------------------------------------------------
#endif