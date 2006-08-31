//------------------------------------------------------------------------------
//  loader/environmentloader.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "loader/environmentloader.h"
#include "db/query.h"
#include "db/server.h"
#include "kernel/nfileserver2.h"
#include "properties/environmentgraphicsproperty.h"
#include "properties/environmentcollideproperty.h"
#include "managers/factorymanager.h"
#include "attr/attributes.h"
#include "properties/pathanimproperty.h"
#include "properties/graphicsproperty.h"
#include "foundation/factory.h"
#include "physics/server.h"
#include "physics/meshshape.h"
#include "game/entity.h"
#include "managers/entitymanager.h"
#include "loader/server.h"
#include "util/segmentedgfxutil.h"

namespace Loader
{
using namespace Properties;
using namespace Game;
using namespace Managers;
using namespace Util;

ImplementRtti(Loader::EnvironmentLoader, Loader::EntityLoaderBase);
ImplementFactory(Loader::EnvironmentLoader);

//------------------------------------------------------------------------------
/**
*/
EnvironmentLoader::EnvironmentLoader()
{
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentLoader::~EnvironmentLoader()
{
}

//------------------------------------------------------------------------------
/**
    This method checks if a collide mesh for the provided resource
    name exists.
*/
bool
EnvironmentLoader::HasCollide(const nString& resName)
{
    nString filename;
    filename.Format("meshes:%s_c_0.nvx2", resName.Get());
    if (nFileServer2::Instance()->FileExists(filename)) {
        return true;
    }
    filename.Format("meshes:%s_c_0.n3d2", resName.Get());
    return (nFileServer2::Instance()->FileExists(filename));
}

//------------------------------------------------------------------------------
/**
    This method checks if a physics file exists for the provided resource
    name.
*/
bool
EnvironmentLoader::HasPhysics(const nString& resName)
{
    nString filename;
    filename.Format("physics:%s.xml", resName.Get());
    return nFileServer2::Instance()->FileExists(filename);
}

//------------------------------------------------------------------------------
/**
*/
bool
EnvironmentLoader::Load(Db::Reader* dbReader)
{
    bool hasEnvironmentNodes = false;
    const nString envCategory = "_Environment";

    // create a segmented gfx util object
    SegmentedGfxUtil segGfxUtil;

    // create a single environment graphics and collide property for all
    // static environment objects, this reduces clutter in the game entity pool
    Ptr<EnvironmentGraphicsProperty> gfxProperty = (EnvironmentGraphicsProperty*) FactoryManager::Instance()->CreateProperty("EnvironmentGraphicsProperty");
    Ptr<EnvironmentCollideProperty> collideProperty = (EnvironmentCollideProperty*) FactoryManager::Instance()->CreateProperty("EnvironmentCollideProperty");

    // for each row...
    int rowIndex;
    int numRows = dbReader->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        dbReader->SetToRow(rowIndex);

        // get category of current row...
        nString category = dbReader->GetString(Attr::_Category);
        if (envCategory == category)
        {
            const nString& resName = dbReader->GetString(Attr::Graphics);
            this->UpdateProgressIndicator(resName);
            if (dbReader->HasAttr(Attr::AnimPath))
            {
                // if the AnimPath attribute exists, create an animated entity
                this->CreateAnimatedEntity(dbReader);
            }
            else if (this->HasPhysics(resName) && !this->HasCollide(resName))
            {
                // if physics and no static collision exists, create physics entity
                this->CreatePhysicsEntity(dbReader);
            }
            else
            {
                // else create vanilla pooled environment entity
                hasEnvironmentNodes = true;
                matrix44 worldMatrix = dbReader->GetMatrix44(Attr::Transform);

                // create graphics entity(s) and attach to graphics property
                nArray<Ptr<Graphics::Entity>> gfxEntities = segGfxUtil.CreateAndSetupGraphicsEntities(resName, worldMatrix, false);
                gfxProperty->AddGraphicsEntities(gfxEntities);

                // check if a collide mesh file exists for the object
                if (this->HasCollide(resName))
                {
                    // collide mesh exists, create a mesh shape node and attach to collide property
                    // FIXME: material type should be assignable in Maya!!
                    nString collideFilename;
                    collideFilename.Format("meshes:%s_c_0.nvx2", resName.Get());
                    Ptr<Physics::MeshShape> meshShape = Physics::Server::Instance()->CreateMeshShape(worldMatrix,
                                                        Physics::MaterialTable::StringToMaterialType("Soil"),
                                                        collideFilename);
                    collideProperty->AddShape(meshShape);
                }
            }
        }
    }

    // create a single game entity for all static environment objects
    // and add the two environment properties
    if (hasEnvironmentNodes)
    {
        Ptr<Game::Entity> gameEntity = FactoryManager::Instance()->CreateEntityByClassName("Entity");
        gameEntity->AttachProperty(gfxProperty);
        gameEntity->AttachProperty(collideProperty);
        EntityManager::Instance()->AttachEntity(gameEntity);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This method is called if an environment with a path animation is
    encountered. In this case we cannot put the object into the
    big environment pool, but instead must create a unique game entity.
*/
void
EnvironmentLoader::CreateAnimatedEntity(Db::Reader* dbReader)
{
    FactoryManager* factory = FactoryManager::Instance();

    // create a raw game entity
    Entity* gameEntity = factory->CreateEntityByClassName("Entity");

    // attach required properties
    Ptr<Property> graphicsProperty = factory->CreateProperty("GraphicsProperty");
    Ptr<Property> pathAnimProperty = factory->CreateProperty("PathAnimProperty");
    gameEntity->AttachProperty(graphicsProperty);
    gameEntity->AttachProperty(pathAnimProperty);

    // setup attributes directly from database
    gameEntity->SetAttr(dbReader->GetAttr(Attr::GUID));
    gameEntity->LoadAttributesFromDatabase();

    // attach game entity to world
    EntityManager::Instance()->AttachEntity(gameEntity);
}

//------------------------------------------------------------------------------
/**
    This method creates a game entity with graphics and physics properties
    attached. This is a usual passive, bouncing, rolling game entity.
*/
void
EnvironmentLoader::CreatePhysicsEntity(Db::Reader* dbReader)
{
    FactoryManager* factory = FactoryManager::Instance();

    // create a game entity
    Entity* gameEntity = factory->CreateEntityByClassName("Entity");

    // attach required properties (NOTE: the order of attachment is
    // important in this case)
    Ptr<Property> physicsProperty  = factory->CreateProperty("PhysicsProperty");
    Ptr<Property> graphicsProperty = factory->CreateProperty("GraphicsProperty");
    gameEntity->AttachProperty(physicsProperty);
    gameEntity->AttachProperty(graphicsProperty);

    // setup attributes directly from database
    gameEntity->SetAttr(dbReader->GetAttr(Attr::GUID));
    gameEntity->LoadAttributesFromDatabase();

    // attach game entity to world
    EntityManager::Instance()->AttachEntity(gameEntity);
}

//------------------------------------------------------------------------------
/**
    Update the progress indicator...
*/
void
EnvironmentLoader::UpdateProgressIndicator(const nString& resName)
{
    Loader::Server* loaderServer = Loader::Server::Instance();
    nString progressText;
    progressText.Format("Loading env object '%s'...", resName.Get());
    loaderServer->SetProgressText(progressText);
    loaderServer->AdvanceProgress(1);
    loaderServer->UpdateProgressDisplay();
}

} // namespace Loader
