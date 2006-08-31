#ifndef LOADER_ENVIRONMENTLOADER_H
#define LOADER_ENVIRONMENTLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::EnvironmentLoader

    Helper class which loads all the environment objects into a level.
    Called by Loader::LevelLoader.

    (C) 2006 RadonLabs GmbH
*/
#include "loader/entityloaderbase.h"
#include "db/query.h"

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
    virtual bool Load(Db::Reader* dbReader);

private:
    /// return true if collide mesh file exists for the resource name
    bool HasCollide(const nString& resName);
    /// return true if physics file exists for the resource name
    bool HasPhysics(const nString& resName);
    /// create a special animated environment entity
    void CreateAnimatedEntity(Db::Reader* dbReader);
    /// create a game entity with graphics and physics
    void CreatePhysicsEntity(Db::Reader* dbReader);
    /// update the progress indicator
    void UpdateProgressIndicator(const nString& resName);
};

RegisterFactory(EnvironmentLoader);

} // namespace Loader
//------------------------------------------------------------------------------
#endif
