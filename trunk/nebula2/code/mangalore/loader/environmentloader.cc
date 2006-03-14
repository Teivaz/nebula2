//------------------------------------------------------------------------------
//  loader/environmentloader.cc
//  (C) 2005 Radon Labs GmbH
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

namespace Loader
{
using namespace Properties;
using namespace Game;
using namespace Managers;

ImplementRtti(EnvironmentLoader, EntityLoaderBase);
ImplementFactory(EnvironmentLoader);

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
    return nFileServer2::Instance()->FileExists(filename);
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
EnvironmentLoader::Load(const nString& levelName)
{
    bool hasEnvironmentNodes = false;

    // create a query which gives us all environment object in the level
    Ptr<Db::Query> query = Db::Server::Instance()->CreateLevelCategoryQuery(levelName, "_Environment");
    query->Execute();

    // create a single environment graphics and collide property for all
    // static environment objects, this reduces clutter in the game entity pool
    Ptr<EnvironmentGraphicsProperty> gfxProperty = (EnvironmentGraphicsProperty*) FactoryManager::Instance()->CreateProperty("EnvironmentGraphicsProperty");
    Ptr<EnvironmentCollideProperty> collideProperty = (EnvironmentCollideProperty*) FactoryManager::Instance()->CreateProperty("EnvironmentCollideProperty");

    // for each object...
    int entityIndex;
    int numEntities = query->GetNumRows();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        const nString& resName = query->GetAttr(Attr::Graphics, entityIndex).GetString();
        if (query->HasAttr(Attr::AnimPath, entityIndex))
        {
            // if the AnimPath attribute exists, create an animated entity
            CreateAnimatedEntity(query, entityIndex);
        }
        else if (HasPhysics(resName) && !HasCollide(resName))
        {
            // if physics and no static collision exists, create physics entity
            CreatePhysicsEntity(query, entityIndex);
        }
        else
        {
            // else create vanilla pooled environment entity
            hasEnvironmentNodes = true;
            matrix44 worldMatrix = query->GetAttr(Attr::Transform, entityIndex).GetMatrix44();

            // create graphics entity and attach to graphics property
			Ptr<Graphics::Entity> gfxEntity = Graphics::Entity::Create();
            gfxEntity->SetTransform(worldMatrix);
            gfxEntity->SetResourceName(resName);
            gfxProperty->AddGraphicsEntity(gfxEntity);

            // check if a collide mesh file exists for the object
            if (HasCollide(resName))
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
EnvironmentLoader::CreateAnimatedEntity(Db::Query* query, int queryRowIndex)
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
    gameEntity->SetAttr(query->GetAttr(Attr::GUID, queryRowIndex));
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
EnvironmentLoader::CreatePhysicsEntity(Db::Query* query, int queryRowIndex)
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
    gameEntity->SetAttr(query->GetAttr(Attr::GUID, queryRowIndex));
    gameEntity->LoadAttributesFromDatabase();

    // attach game entity to world
    EntityManager::Instance()->AttachEntity(gameEntity);
}

}; // namespace Loader