#ifndef LOADER_ENVIRONMENTLOADER_H
#define LOADER_ENVIRONMENTLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::EnvironmentLoader

    Helper class which loads all the environment objects into a level.
    Called by Loader::LevelLoader.

    (C) 2005 Radon Labs GmbH
*/
#include "util/nstring.h"
#include "db/query.h"
#include "loader/entityloaderbase.h"

//------------------------------------------------------------------------------
namespace Loader
{
class EnvironmentLoader : public EntityLoaderBase
{
    DeclareRtti;
    DeclareFactory(EnvironmentLoader);
public:
    /// constructor
    EnvironmentLoader();
    /// destructor
    ~EnvironmentLoader();
    /// load environment objects into the level
    virtual bool Load(const nString& levelName);

private:
    /// return true if collide mesh file exists for the resource name
    bool HasCollide(const nString& resName);
    /// return true if physics file exists for the resource name
    bool HasPhysics(const nString& resName);
    /// create a special animated environment entity
    void CreateAnimatedEntity(Db::Query* query, int queryRowIndex);
    /// create a game entity with graphics and physics
    void CreatePhysicsEntity(Db::Query* query, int queryRowIndex);
};
RegisterFactory(EnvironmentLoader);

} // namespace Loader
//------------------------------------------------------------------------------
#endif
