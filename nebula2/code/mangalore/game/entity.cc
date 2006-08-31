//------------------------------------------------------------------------------
//  game/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "attr/attributes.h"
#include "game/attrset.h"
#include "game/entity.h"
#include "mathlib/transform44.h"
#include "foundation/factory.h"
#include "application/app.h"

namespace Game
{
ImplementRtti(Game::Entity, Foundation::RefCounted);
ImplementFactory(Game::Entity);

uint Entity::uniqueIdCounter = 0;

//------------------------------------------------------------------------------
/**
*/
Entity::Entity() :
    uniqueId(++uniqueIdCounter),
    entityPool(LivePool),
    activated(false),
    isInOnActivate(false),
    isInOnDeactivate(false)
{
    this->dispatcher = Message::Dispatcher::Create();
    this->dbEntity = Db::Entity::Create();
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    this->dbEntity = 0;
    this->dispatcher = 0;
}

//------------------------------------------------------------------------------
/**
    This method sets up the entity properties. Overwrite this method
    in entity subclasses to create

    This method sets up the entity. The method is called from within
    OnActivate(). Override the method for different or additional setup tasks.
*/
void
Entity::SetupProperties()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method cleans up the entity. It is called from OnDeactivate().
    Override this method to cleanup any subclass specific stuff when the
    entity is removed from the level.
*/
void
Entity::CleanupProperties()
{
    // deactivate and remove all properties
    this->DeactivateProperties();
    while (this->properties.Size() > 0)
    {
        this->RemoveProperty(this->properties.Back());
    }
}

//------------------------------------------------------------------------------
/**
    CAREFUL! THIS METHOD IS SLOW.

    This method loads the entity attributes from the world database
    and attached them to the entity. To identify the entity in the database,
    a valid GUID attribute must be attached to the entity.
    Existing attributes will be overwritten (if they are in the database) or
    left untouched (if they are not in the database). To implement post-loading
    initialization actions, please be aware of the Entity::OnLoad(),
    Property::OnLoad() and Manager::OnLoad() methods. These methods are called
    after all entities have been loaded (this is important if entity need other
    entities for their setup).
*/
void
Entity::LoadAttributesFromDatabase()
{
    this->dbEntity->Load();
}

//------------------------------------------------------------------------------
/**
    Fast version to load entity attributes. A Db reader only does ONE
    query on the database for entities in the level, so this should
    be much faster then LoadAttributesFromDatabase() if many entities
    are loaded (like loading a level or a savegame). The reader has to
    be positioned on the entity data.
*/
void
Entity::LoadAttributesFromDbReader(Db::Reader* dbReader)
{
    this->dbEntity->LoadFromReader(dbReader);
}

//------------------------------------------------------------------------------
/**
    CAREFUL! THIS METHOD IS SLOW.

    This method saves all entity attributes back into the database.
    It is usually flush the current save state back into the database.
    If you want to do any work before saving happens please have a look
    at the methods Entity::OnSave(), Property::OnSave() and Manager::OnSave().
    These methods are called before saving happens (first the OnSave() method
    is called on all entities, only after that, attributes are saved to the
    database).

    To make this method work as expected, the entity must have a valid GUID
    attribute attached which identifies the entity in the world database.
    Entity without a GUID will simply not be saved. If an entry with this
    GUID already exists in the world database, it will be overwritten with
    the new attribute values, otherwise a new entry will be created in the
    world database. New attribute columns in the world database are created
    as expected.

    If the entity was created from template force to save all attributes once.
*/
void
Entity::SaveAttributesToDatabase()
{
    this->dbEntity->Save();
}

//------------------------------------------------------------------------------
/**
    Fast version to save entity attributes. A Db writer caches the attributes
    for all entities and does only one access to the database for all entities.
*/
void
Entity::SaveAttributesToDbWriter(Db::Writer *dbWriter)
{
    this->dbEntity->SaveToWriter(dbWriter);
}

//------------------------------------------------------------------------------
/**
    Called when the game entity has been attached to a game level object.
    This will attach contained subsystem entities to their respective
    subsystems.

    @param  l   pointer to a level object to which the entity was attached

    - 01-Apr-05 floh    graphics entity now created after physics entity
*/
void
Entity::OnActivate()
{
    n_assert(!this->activated);
    n_assert(!this->isInOnActivate);
    this->isInOnActivate = true;
    n_assert(!this->isInOnDeactivate);

    // attach dispatcher to message server
    Message::Server::Instance()->RegisterPort(this->dispatcher);

    // setup properties
    this->SetupProperties();

    // activate all properties
    this->ActivateProperties();

    // set activated flag
    this->activated = true;
    this->isInOnActivate = false;
}

//------------------------------------------------------------------------------
/**
    Called when the game entity has been removed from the game level object.
    This will remove any contained subsystem entities from their subsystems.
*/
void
Entity::OnDeactivate()
{
    n_assert(this->activated);
    n_assert(!this->isInOnDeactivate);
    this->isInOnDeactivate = true;
    n_assert(!this->isInOnActivate);

    // cleanup properties
    this->CleanupProperties();

    // detach dispatcher from message server
    Message::Server::Instance()->UnregisterPort(this->dispatcher);

    // clear activated flag
    this->activated = false;
    this->isInOnDeactivate = false;
}

//------------------------------------------------------------------------------
/**
    Called on game entities at the begin of the frame.
*/
void
Entity::OnBeginFrame()
{
    n_assert(LivePool == this->entityPool);

    // call properties
    int i;
    int num = this->properties.Size();
    for (i = 0; this->activated && i < num; i++)
    {
        this->properties[i]->OnBeginFrame();
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities before movement.
*/
void
Entity::OnMoveBefore()
{
    n_assert(LivePool == this->entityPool);

    // call properties
    int i;
    int num = this->properties.Size();
    for (i = 0; this->activated && i < num; i++)
    {
        this->properties[i]->OnMoveBefore();
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities after movement.
*/
void
Entity::OnMoveAfter()
{
    n_assert(LivePool == this->entityPool);

    // call properties
    int i;
    int num = this->properties.Size();
    for (i = 0; this->activated && i < num; i++)
    {
        this->properties[i]->OnMoveAfter();
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities before rendering.
*/
void
Entity::OnRender()
{
    n_assert(LivePool == this->entityPool);

    // call properties
    int i;
    int num = this->properties.Size();
    for (i = 0; this->activated && i < num; i++)
    {
        this->properties[i]->OnRender();
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities before rendering.
*/
void
Entity::OnRenderDebug()
{
    n_assert(LivePool == this->entityPool);

    // call properties
    int i;
    int num = this->properties.Size();
    for (i = 0; this->activated && i < num; i++)
    {
        this->properties[i]->OnRenderDebug();
    }
}


//------------------------------------------------------------------------------
/**
    This method is called after the game world has been loaded from the
    database. At the time when this method is called all entities
    in the world have already been created and their attributes have been
    loaded from the database.

    This method reads the entity attributes from the world database. A
    valid GUID attribute must exist on the entity for identification in
    the database. After the attributes are loaded from the world database,
    the Property::OnLoad() method will be called on all attached properties.
*/
void
Entity::OnLoad()
{
    // let properties do their post-loading-stuff
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        this->properties[i]->OnLoad();
    }
}

//------------------------------------------------------------------------------
/**
    This method is called in 2 cases:

    When a level is loaded it is called on all entities after OnLoad when the
    complete world already exist.

    When a entity is created at runtime (while a level is active) OnStart is
    called after the entity is attached to level.
*/
void
Entity::OnStart()
{
    // let propreties do there OnStart init stuff
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        this->properties[i]->OnStart();
    }
}

//------------------------------------------------------------------------------
/**
    This method writes the current entity attributes back to the world database.
    At least a valid GUID attribute must exist on the entity for
    identification. If no entry exists yet in the database for the
    entity, a new one will be created. Before the attributes are written back
    to the database, the method Property::OnSave() will be called on each
    attached property.
*/
void
Entity::OnSave()
{
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        this->properties[i]->OnSave();
    }
}

//------------------------------------------------------------------------------
/**
    Attach a new property to the entity.
*/
void
Entity::AttachProperty(Property* prop)
{
    n_assert(0 != prop);

    // only add the entity if it is compatible with our entity pool
    // and it should be added to entities in the current state.
    if (prop->GetActiveEntityPools() & this->GetEntityPool())
    {
        nString currState = Application::App::Instance()->GetCurrentState();
        if (prop->IsActiveInState(currState))
        {
            this->properties.Append(prop);
            prop->SetEntity(this);
            prop->SetupDefaultAttributes();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Remove a property from the entity. If the property is not attached to the entity,
    a hard error will be thrown.
*/
void
Entity::RemoveProperty(Property* prop)
{
    n_assert(0 != prop);
    int index = this->properties.FindIndex(prop);
    if (-1 != index)
    {
        if (this->properties[index]->IsActive())
        {
            this->properties[index]->OnDeactivate();
        }
        this->properties[index]->ClearEntity();
        this->properties.Erase(index);
    }
    else
    {
        n_error("Entity::RemoveProperty: Property '%s' does not exist on entity!", prop->GetClassName());
    }
}

//------------------------------------------------------------------------------
/**
    This method calls OnActivate() on all attached properties.
*/
void
Entity::ActivateProperties()
{
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        n_assert(!this->properties[i]->IsActive());
        this->dispatcher->AttachPort(this->properties[i]);
        this->properties[i]->OnActivate();
    }
}

//------------------------------------------------------------------------------
/**
    This method calls OnDeactivate() on all attached properties.
*/
void
Entity::DeactivateProperties()
{
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        n_assert(this->properties[i]->IsActive());
        this->dispatcher->RemovePort(this->properties[i]);
        this->properties[i]->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
    Find pointer to property by property id.
*/
Property*
Entity::FindProperty(const Foundation::Rtti& propId) const
{
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        if (this->properties[i]->IsA(propId))
        {
            return this->properties[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Return true if property exists on entity.
*/
bool
Entity::HasProperty(const Foundation::Rtti& propId) const
{
    return (0 != this->FindProperty(propId));
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddAttributeSet(AttrSet* attrSet)
{
    n_assert(attrSet != 0);
    if (!this->HasAttributeSet(attrSet))
    {
        attrSet->SetupEntity(this);
        this->attrSets.PushBack(attrSet);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::HasAttributeSet(const AttrSet* attrSet) const
{
    n_assert(attrSet != 0);
    int i;
    for (i = 0; i < this->attrSets.Size(); i++)
    {
        if (this->attrSets[i]->IsInstanceOf(*attrSet->GetRtti()))
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AttachPort(Message::Port* port)
{
    n_assert(port);
    n_assert(this->dispatcher);

    this->dispatcher->AttachPort(port);
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::RemovePort(Message::Port* port)
{
    n_assert(port);
    n_assert(this->dispatcher);

    this->dispatcher->RemovePort(port);
}

} // namespace Game
