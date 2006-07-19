//------------------------------------------------------------------------------
//  managers/factorymanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "foundation/factory.h"
#include "kernel/nguid.h"
#include "managers/factorymanager.h"
#include "managers/setupmanager.h"
#include "game/property.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "db/query.h"
#include "db/server.h"
#include "kernel/nfileserver2.h"
#include "properties/environmentgraphicsproperty.h"
#include "properties/environmentcollideproperty.h"
#include "properties/physicsproperty.h"
#include "properties/timeproperty.h"
#include "properties/lightproperty.h"
#include "properties/inputproperty.h"
#include "properties/graphicsproperty.h"
#include "properties/cameraproperty.h"
#include "properties/actorphysicsproperty.h"
#include "properties/videocameraproperty.h"
#include "properties/videocameraproperty2.h"
#include "properties/chasecameraproperty.h"
#include "properties/pointnclickinputproperty.h"
#include "properties/actorgraphicsproperty.h"
#include "properties/pathanimproperty.h"
#include "properties/posteffectproperty.h"
#include "properties/mousegripperproperty.h"
#include "properties/actoranimationproperty.h"
#include "properties/simplecameraproperty.h"

namespace Managers
{
ImplementRtti(Managers::FactoryManager, Game::Manager);
ImplementFactory(Managers::FactoryManager);

FactoryManager* FactoryManager::Singleton = 0;

//using namespace Properties;
using namespace Game;

//------------------------------------------------------------------------------
/**
*/
FactoryManager::FactoryManager()
{
    n_assert(0 == Singleton);
    Singleton = this;

    if (!this->ParseBluePrints())
    {
        n_error("Managers::FactoryManager: Error parsing data:tables/blueprints.xml!");
    }
}

//------------------------------------------------------------------------------
/**
*/
FactoryManager::~FactoryManager()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    This reads all entity templates from the database and caches them for
    faster execution later on.
*/
void
FactoryManager::OnActivate()
{
    Manager::OnActivate();

    // configure and execute a database query
    if (Db::Server::Instance()->IsOpen())
    {
        this->cachedTemplateQuery = Db::Query::Create();
        this->cachedTemplateQuery->SetTableName("_Entities");
        this->cachedTemplateQuery->AddWhereAttr(Db::Attribute(Attr::_Type, "TEMPLATE"));
        this->cachedTemplateQuery->BuildSelectStatement();
        this->cachedTemplateQuery->Execute();
        if (this->cachedTemplateQuery->GetNumRows() == 0)
        {
            n_error("FactoryManager::OnActivate(): no templates found in world database!");
        }
        n_assert(this->cachedTemplateQuery->HasColumn(Attr::_Category));
    }
}

//------------------------------------------------------------------------------
/**
    This just releases the template cache query object.
*/
void
FactoryManager::OnDeactivate()
{
    this->cachedTemplateQuery = 0;
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    Create a raw entity by its C++ class name name. This method should be 
    extended by subclasses if a Mangalore application implements new
    Game::Entity subclasses.
*/
Entity*
FactoryManager::CreateEntityByClassName(const nString& cppClassName) const
{
    if ("Entity" == cppClassName) 
    {
        return Entity::Create();
    }
    else
    {
        n_error("Managers::FactoryManager::CreateEntity(): unknown entity class name '%s'!", cppClassName.Get());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Create an entity from its category name. The category name is looked
    up in the blueprints.xml file to check what properties must be attached
    to the entity. All required properties will be attached, and all
    attributes will be initialised in their default state. A new unique
    id will be created for the entity and stored in the GUID attribute.

    No new database entry will be created until the Entity::Save() method
    is called on the new entity.
*/
Entity*
FactoryManager::CreateEntityByCategory(const nString& categoryName, Entity::EntityPool entityPool) const
{
    // find blueprint index
    int i = this->FindBluePrint(categoryName);
    if (-1 != i)
    {
        // create raw entity
        Entity* entity = this->CreateEntityByClassName(this->bluePrints[i].cppClass);
        entity->SetDispatcher(this->CreateDispatcher());
        entity->SetEntityPool(entityPool);
        
        // add properties as described in blueprints.xml
        this->AddProperties(entity, categoryName);

        // create a new guid for the entity
        nGuid guid;
        guid.Generate();
        entity->SetString(Attr::_Category, categoryName);
        entity->SetString(Attr::GUID, guid.Get());
        return entity;
    }
    else
    {
        n_error("Managers::FactoryManager::CreateEntityByCategory(%s): category not found in blueprints.xml!", categoryName.Get());
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Create an entity from a template in the database. The template is
    defined by category name and the template name. This will create
    a complete entity with properties and attributes initialized to the
    values from the template.

    The template database entry is identified by the following SQL
    statement:

    @code
    SELECT *
    FROM '_Entities'
    WHERE ROWTYPE='TEMPLATE' AND
    _Category='[categoryName]' AND
    Id='[templateName]'
    @endcode

    A new GUID will be assigned to the entity.
    No new database entry will be created until the Entity::Save() method
    is called on the new entity.

    To save all attribute the IsFromTemplate flag will be set on the entity,
    so the entity will store ALL attributes on 1st save.
*/
Entity*
FactoryManager::CreateEntityByTemplate(const nString& categoryName, const nString& templateName, Entity::EntityPool entityPool) const
{
	n_assert(categoryName.IsValid());
	n_assert(templateName.IsValid());
    n_assert(this->cachedTemplateQuery.isvalid());

    // find the category in the cached template query
    int rowIndex;
    int numRows = this->cachedTemplateQuery->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        if ((this->cachedTemplateQuery->GetString(Attr::_Category, rowIndex) == categoryName) &&
            (this->cachedTemplateQuery->GetString(Attr::Id, rowIndex) == templateName))
        {
            break;
        }
    }
    if (rowIndex == numRows)
    {
        // fallthrough: error!
        n_error("Managers::FactoryManager::CreateEntityByTemplate(): no template entity with category='%s' and template='%s' in database!",
            categoryName.Get(), templateName.Get());
        return 0;
    }

    // create entity by category, this will just add properties
    Game::Entity* newEntity = this->CreateEntityByCategory(categoryName, entityPool);

    // transfer attributes to entity
    int colIndex;
    int numColumns = this->cachedTemplateQuery->GetNumColumns();
    for (colIndex = 0; colIndex < numColumns; colIndex++)
    {
        if (this->cachedTemplateQuery->GetAttr(colIndex, rowIndex).IsValid())
        {
            newEntity->SetAttr(this->cachedTemplateQuery->GetAttr(colIndex, rowIndex));
        }
    }

    // setup as instance, also need to overwrite GUID, since the
    // new GUID value from CreateEntityByCategory() has been overwritten by
    // the previous loop
    newEntity->SetString(Attr::_Type, "INSTANCE");
    nGuid guid;
    guid.Generate();
    newEntity->SetString(Attr::GUID, guid.Get());
    
    // set current level
    newEntity->SetString(Attr::_Level, Managers::SetupManager::Instance()->GetCurrentLevel());

    return newEntity;
}

//------------------------------------------------------------------------------
/**
    Create a entity as a clone of a existing one. A new GUID will be assigned.
*/
Entity*
FactoryManager::CreateEntityByEntity(Entity* templateEntity, Entity::EntityPool entityPool) const
{
    n_assert(templateEntity != 0);
    n_assert(templateEntity->HasAttr(Attr::_Category) && templateEntity->GetString(Attr::_Category).IsValid());
    n_assert(templateEntity->HasAttr(Attr::Id) && templateEntity->GetString(Attr::Id).IsValid());

    Entity* newEntity = this->CreateEntityByTemplate(templateEntity->GetString(Attr::_Category), templateEntity->GetString(Attr::Id));
    n_assert(newEntity);
 
    // copy all attributes from template (skip the GUID!)
    const nArray<Db::Attribute>& templateAttrs = templateEntity->GetAttrs();
    int i;
    int num = templateAttrs.Size();
    for (i = 0; i < num; i++)
    {
        if (templateAttrs[i].GetAttributeID() != Attr::GUID)
        {
            newEntity->SetAttr(templateAttrs[i]);
        }
    }

    return newEntity;    
}

//------------------------------------------------------------------------------
/**
    This will 'load' a new entity from the world database (AKA making the
    entity 'live') and place it in the given entity pool (Live or Sleeping).
    This will create a new entity, attach properties as described by
    blueprints.xml, and update the entity attributes from the database.
    Changes to attributes can later be written back to the
    database by calling the Entity::Save() method.

    NOTE: this method will not call the Entity::OnLoad() method, which may be
    required to finally initialize the entity. The OnLoad() method expects
    that all other entities in the level have already been loaded, so this
    must be done after loading in a separate pass.

    FIXME: This method does 2 complete queries on the database!!
*/
Entity*
FactoryManager::CreateEntityByKeyAttr(const Db::Attribute& key, Entity::EntityPool entityPool) const
{
    // get the entity attributes from the world database
    Ptr<Db::Query> dbQuery = Db::Server::Instance()->CreateQuery();
    dbQuery->SetTableName("_Entities");
    dbQuery->AddWhereAttr(key);
    dbQuery->AddWhereAttr(Db::Attribute(Attr::_Type, nString("INSTANCE")));
    dbQuery->AddResultAttr(Attr::_Category);
    dbQuery->AddResultAttr(Attr::GUID);
    dbQuery->BuildSelectStatement();
    if (dbQuery->Execute())
    {
        if (dbQuery->GetNumRows() == 0)
        {
            n_error("Managers::FactoryManager::CreateEntityByKeyAttr(): no key '%s=%s' in world database!", key.GetName().Get(), key.AsString().Get());
            return 0;
        }
        else if (dbQuery->GetNumRows() > 1)
        {
            n_error("Managers::FactoryManager::CreateEntityByKeyAttr(): more then one entry with key '%s=%s' in world database!", key.GetName().Get(), key.AsString().Get());
            return 0;
        }
        else
        {
            // get the category (blueprint) from the query result
            nString categoryName = dbQuery->GetString(Attr::_Category, 0);
            nString guid = dbQuery->GetString(Attr::GUID, 0);

            // create entity by category, this will attach properties and
            // initialize attributes to their initial state
            Entity* newEntity = this->CreateEntityByCategory(categoryName, entityPool);

            // now overwrite the guid attribute so the entity knows where to initialize from
            newEntity->SetString(Attr::GUID, guid);

            // finally, let the entity load its attributes from the database
            newEntity->LoadAttributesFromDatabase();

            // that's it, the entity is complete
            return newEntity;
        }
    }
    // fallthrough, something went wrong
    n_error("Managers::FactoryManager::CreateEntityByKeyAttr(): failed to load entity with key '%s=%s' from world database!", key.GetName().Get(), key.AsString().Get());
    return 0;
}

//------------------------------------------------------------------------------
/**
    This will 'load' a new entities from the world database (AKA making the
    entity 'live') and place it in the given entity pool (Live or Sleeping). 
    This will create new entities, attach properties as described by 
    blueprints.xml, and update the entities attributes from the database. 
    Changes to attributes can later be written back to the
    database by calling the Entity::Save() method.

    NOTE: this method will not call the Entity::OnLoad() method, which may be
    required to finally initialize the entity. The OnLoad() method expects
    that all other entities in the level have already been loaded, so this
    must be done after loading in a separate pass.

    FIXME: This method does 1 + numEnities complete queries on the database!!
*/
nArray<Entity*>
FactoryManager::CreateEntitiesByKeyAttrs(const nArray<Db::Attribute>& keys, const nArray<Ptr<Game::Entity> >& filteredEntitys, Entity::EntityPool entityPool, bool failOnDBError) const
{
    nArray<Entity*> entities;

    // get the entity attributes from the world database
    Ptr<Db::Query> dbQuery = Db::Server::Instance()->CreateQuery();
    dbQuery->SetTableName("_Entities");
    dbQuery->AddWhereAttr(Db::Attribute(Attr::_Type, nString("INSTANCE")));
    
    // add attributes as where clause
    int i;
    for (i = 0; i < keys.Size(); i++)
    {
        dbQuery->AddWhereAttr(keys[i]);
    }
    // add filter entitys where not clause
    for (i = 0; i < filteredEntitys.Size(); i++)
    {
        dbQuery->AddWhereAttr(filteredEntitys[i]->GetAttr(Attr::GUID), true); // NOT
    }

    // define results
    dbQuery->AddResultAttr(Attr::_Category);
    dbQuery->AddResultAttr(Attr::GUID);
    dbQuery->BuildSelectStatement();
    
    if (dbQuery->Execute(failOnDBError))
    {
        if (dbQuery->GetNumRows() > 0)
        {
            for (i = 0; i < dbQuery->GetNumRows(); i++)
            {
                // get the category (blueprint) from the query result
                nString categoryName = dbQuery->GetString(Attr::_Category, i);
                nString guid = dbQuery->GetString(Attr::GUID, i);
                
                // create entity by category, this will attach properties and
                // initialize attributes to their initial state
                Entity* newEntity = this->CreateEntityByCategory(categoryName, entityPool);

                // now overwrite the guid attribute so the entity knows where to initialize from
                newEntity->SetString(Attr::GUID, guid);

                // finally, let the entity load its attributes from the database
                newEntity->LoadAttributesFromDatabase();

                // that's it, the entity is complete
                entities.Append(newEntity);
            }
        }
    }
    return entities;
}

//------------------------------------------------------------------------------
/**
    Creates a new entity from the world database using a GUID as key.
    Simply calls CreateEntityByKeyAttr().
*/
Entity*
FactoryManager::CreateEntityByGuid(const nString& guid, Entity::EntityPool entityPool) const
{
    return this->CreateEntityByKeyAttr(Db::Attribute(Attr::GUID, guid), entityPool);
}

//------------------------------------------------------------------------------
/**
    Create an entity from a row in a database reader object. This is the
    fastest way to load many entities because only one database access must
    be done for all entities. This is the way how levels and savegames are loaded.
*/
Entity*
FactoryManager::CreateEntityByDbReader(Db::Reader* dbReader, int rowIndex, Entity::EntityPool entityPool) const
{
    n_assert(0 != dbReader);

    // position the "read cursor"
    dbReader->SetToRow(rowIndex);

    // get the category (blueprint) and GUID
    nString categoryName = dbReader->GetString(Attr::_Category);
    nString guid = dbReader->GetString(Attr::GUID);
    
    // create entity by category, this will attach properties and
    // initialize attributes to their initial state
    Entity* newEntity = this->CreateEntityByCategory(categoryName, entityPool);

    // now overwrite the guid attribute so the entity knows where to initialize from
    newEntity->SetString(Attr::GUID, guid);

    // finally, let the entity load its attributes from the database
    newEntity->LoadAttributesFromDbReader(dbReader);

    // that's it, the entity is complete
    return newEntity;
}

//------------------------------------------------------------------------------
/**
    Create a property by its type name. This method should be extended by
    subclasses if a Mangalore application implements new properties.
*/
Property*
FactoryManager::CreateProperty(const nString& type) const
{
    nString fullType = "Properties::" + type;
    Game::Property* result = (Game::Property*) Foundation::Factory::Instance()->Create(fullType);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
    This method parses the file data:tables/blueprints.xml into
    the bluePrints array.
*/
bool
FactoryManager::ParseBluePrints()
{
    // it is not an error here if blueprints.xml doesn't exist
    if (nFileServer2::Instance()->FileExists("data:tables/blueprints.xml"))
    {
        nStream stream;
        stream.SetFilename("data:tables/blueprints.xml");
        if (stream.Open(nStream::Read))
        {
            // make sure it's a BluePrints file
            if (stream.GetCurrentNodeName() != "BluePrints")
            {
                n_error("FactoryManager::ParseBluePrints(): not a valid blueprints file!");
                return false;
            }
            if (stream.SetToFirstChild("Entity")) do
            {
                BluePrint bluePrint;
                bluePrint.type = stream.GetString("type");
                bluePrint.cppClass = stream.GetString("cppclass");
                if (stream.SetToFirstChild("Property")) do
                {
                    bluePrint.properties.Append(stream.GetString("type"));
                }
                while (stream.SetToNextChild("Property"));
                this->bluePrints.Append(bluePrint);
            }
            while (stream.SetToNextChild("Entity"));
            stream.Close();
            return true;
        }
        else
        {
            n_error("Managers::FactoryManager::ParseBluePrints(): could not open 'data:tables/blueprints.xml'!");
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This method finds a blueprint index by entity type name. Returns
    -1 if blueprint doesn't exist.
*/
int
FactoryManager::FindBluePrint(const nString& entityType) const
{
    int i;
    int num = this->bluePrints.Size();
    for (i = 0;  i < num; i++)
    {
        if (this->bluePrints[i].type == entityType)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    This method checks if a blueprint for the provided entity exists,
    and adds the properties defined in the blue print to the entity. If
    no matching blueprint exists, the entity will not be altered.
*/
void
FactoryManager::AddProperties(Entity* entity, const nString& categoryName) const
{
    n_assert(entity);

    int index = this->FindBluePrint(categoryName);
    if (-1 != index)
    {
        const BluePrint& bluePrint = this->bluePrints[index];
        int i;
        int num = bluePrint.properties.Size();
        for (i = 0; i < num; i++)
        {
            Ptr<Property> prop = this->CreateProperty(bluePrint.properties[i]);
            entity->AttachProperty(prop);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This Methode creates a Message Dispatcher for a Game Entity
*/
Message::Dispatcher*
FactoryManager::CreateDispatcher() const
{
    return Message::Dispatcher::Create();
}
} // namespace Managers
