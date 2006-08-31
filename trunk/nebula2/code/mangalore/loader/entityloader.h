#ifndef LOADER_ENTITYLOADER_H
#define LOADER_ENTITYLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::EntityLoader

    Loader helper for universal game entities. The properties which are
    attached to the entity are described in blueprints.xml, the attributes
    to attach come from the world database.

    (C) 2006 RadonLabs GmbH
*/
#include "loader/entityloaderbase.h"
#include "game/entity.h"

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
    virtual bool Load(Db::Reader* dbReader);

private:
    /// update the progress indicator
    void UpdateProgressIndicator(Game::Entity* gameEntity);
};

RegisterFactory(EntityLoader);

} // namespace Loader
//------------------------------------------------------------------------------
#endif
