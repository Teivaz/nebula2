#ifndef MANAGERS_FACTORYMANAGER_H
#define MANAGERS_FACTORYMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Managers::FactoryManager

    The FactoryManager is responsible for creating new game entities. 
    FactoryManager must be extended by Mangalore applications if the
    application needs new game entity classes.

    The FactoryManager loads the file

    data:tables/entitytypes.xml

    on creation, which contains the construction blueprints for the
    entity types of your application. This file defines entity types
    by the properties which are added to them.

    (C) 2005 Radon Labs GmbH  
*/
#include "game/manager.h"
#include "game/property.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace Managers
{
class FactoryManager : public Game::Manager
{
    DeclareRtti;
	DeclareFactory(FactoryManager);

public:
    /// constructor
    FactoryManager();
    /// destructor
    virtual ~FactoryManager();
    /// get instance pointer
    static FactoryManager* Instance();
    /// create a new raw game entity by type name, extend this method in subclasses for new types
    virtual Game::Entity* CreateEntityByClassName(const nString& cppClassName) const;
    /// create a new entity from its category name
    virtual Game::Entity* CreateEntityByCategory(const nString& categoryName, Game::Entity::EntityPool pool = Game::Entity::LivePool) const;
    /// create a new entity from a database template entry
    virtual Game::Entity* CreateEntityByTemplate(const nString& categoryName, const nString& templateName, Game::Entity::EntityPool = Game::Entity::LivePool) const;
    /// create new entity from world database using any key attribute
    virtual Game::Entity* CreateEntityByKeyAttr(const Db::Attribute& key, Game::Entity::EntityPool pool = Game::Entity::LivePool) const;
    /// create new entity from world database using GUID as key attribute
    virtual Game::Entity* CreateEntityByGuid(const nString& guid, Game::Entity::EntityPool pool = Game::Entity::LivePool) const;
    /// create a new property by type name, extend in subclass for new types
    virtual Game::Property* CreateProperty(const nString& type) const;

private:
    static FactoryManager* Singleton;

protected:
    /// parse entity blueprints file
    bool ParseBluePrints();
    /// find blueprint index by property type
    int FindBluePrint(const nString& entityType) const;
    /// add properties to entity according to blue print
    void AddProperties(Game::Entity* entity, const nString& categoryName) const;

    /// an entity blueprint, these are created by ParseBlueprints()
    struct BluePrint
    {
        nString type;
        nString cppClass;
        nArray<nString> properties;
    };

    nArray<BluePrint> bluePrints;
};

RegisterFactory(FactoryManager);

//------------------------------------------------------------------------------
/**
*/
inline
FactoryManager*
FactoryManager::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

}; // namespace Managers
//------------------------------------------------------------------------------
#endif