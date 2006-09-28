#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Game::Entity

    The new style game entity class. A game entity represents one game object,
    such as an actor, an item, etc...

    The entity class itself doesn't have any game specific attributes or
    functionality. Instead it's configured by attaching properties to the
    entity. Properties add functionality to a game entity, for instance,
    if an entity should be visible, add a GraphicsProperty, if it should
    be audible, add an AudioProperty, and so forth...

    Entities are organized into pools (at the moment there is a LivePool
    and a SleepingPool). LivePool entities are part of the current
    level and function as usual. SleepingPool entities just serve as an
    interface to "sleeping" data in the database. The idea is that live
    entities can just as well be manipulated as entities which exist in
    another location and are not active at the moment, this is also
    a fast cache for database operations.

    Properties must indicate through the virtual GetActiveEntityPools()
    method in which pools they want to be active. Most properties
    are active in both pools, some properties
    (like the GraphicsProperty) make only sense for Live entities.
    Properties where the active entity pools mask doesn't match
    the pool of their entity won't even be added, and as such don't
    cost unnecessary memory and processing time.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "db/entity.h"
#include "message/msg.h"
#include "message/dispatcher.h"
#include "util/nstream.h"
#include "game/property.h"

namespace Message
{
    class Port;
}

namespace Managers
{
    class FactoryManager;
}

//------------------------------------------------------------------------------
namespace Game
{
class AttrSet;

class Entity : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(Entity);

public:
    /// an Id type, used to identify entities
    typedef unsigned int EntityId;
    /// entity pool types
    enum EntityPool
    {
        LivePool = (1<<0),          // a normal "live" entity
        SleepingPool = (1<<1),      // a sleeping (external) entity which is just a front-end to database data
    };

    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();

    //=== basics ===

    /// get unique id of entity
    EntityId GetUniqueId() const;
    /// get pool this entity is part of
    EntityPool GetEntityPool() const;
    /// send a synchronous message to the entity
    void SendSync(Message::Msg* msg);
    /// send an asynchronous message to the entity
    void SendAsync(Message::Msg* msg);
    /// SLOW!!! load entity attributes from db, entity needs valid GUID attr
    void LoadAttributesFromDatabase();
    /// SLOW!!! save entity attributes back to db, entity needs valid GUID attr
    void SaveAttributesToDatabase();
    /// FAST FOR MANY ENTITIES!!! load entity attributes from a db reader, entity needs valid GUID attr
    void LoadAttributesFromDbReader(Db::Reader* dbReader);
    /// FAST FOR MANY ENTITIES!!! save entity attributes from a db reader, entity needs valid GUID attr
    void SaveAttributesToDbWriter(Db::Writer* dbWriter);

    //=== callbacks ===

    /// called when attached to world
    virtual void OnActivate();
    /// called when removed from world
    virtual void OnDeactivate();
    /// called at the beginning of the frame
    virtual void OnBeginFrame();
    /// called before movement
    virtual void OnMoveBefore();
    /// called after movement
    virtual void OnMoveAfter();
    /// called before rendering
    virtual void OnRender();
    /// debug rendering called before rendering
    virtual void OnRenderDebug();
    /// called after loading from database has happened
    virtual void OnLoad();
    /// called when the entity starts to live in the complete world
    virtual void OnStart();
    /// called before saving from database happens
    virtual void OnSave();

    //=== access to the embedded dispatcher (to be able to listen to msgs a entities gets) ===
    /// attach to the embedded dispatcher message port
    void AttachPort(Message::Port* port);
    /// remove from the embedded dispatcher message port
    void RemovePort(Message::Port* port);

    //=== properties ===

    /// attach a property to the entity
    void AttachProperty(Property* prop);
    /// remove a property from the entity
    void RemoveProperty(Property* prop);
    /// return true if a property exists on the entity
    bool HasProperty(const Foundation::Rtti& propId) const;
    /// find property pointer by property id
    Property* FindProperty(const Foundation::Rtti& propId) const;

    //=== attributes ===

    /// return true if entity has an attribute
    bool HasAttr(Attr::AttributeID attrId) const;

    /// set a generic attribute
    void SetAttr(const Db::Attribute& attr);
    /// get a generic attribute
    const Db::Attribute& GetAttr(Attr::AttributeID attrId) const;
    /// direct read access to all attributes
    const nArray<Db::Attribute>& GetAttrs() const;
    /// set string attribute on the entity
    void SetString(Attr::StringAttributeID attrId, const nString& s);
    /// get string attribute from the entity
    nString GetString(Attr::StringAttributeID attrId) const;
    /// set int attribute on the entity
    void SetInt(Attr::IntAttributeID attrId, int i);
    /// get int attribute from the entity
    int GetInt(Attr::IntAttributeID attrId) const;
    /// set float attribute on the entity
    void SetFloat(Attr::FloatAttributeID attrId, float f);
    /// get float attribute from the entity
    float GetFloat(Attr::FloatAttributeID attrId) const;
    /// set bool attribute on the entity
    void SetBool(Attr::BoolAttributeID attrId, bool b);
    /// get bool attribute from the entity
    bool GetBool(Attr::BoolAttributeID attrId) const;
    /// set vector3 attribute on the entity
    void SetVector3(Attr::Vector3AttributeID attrId, const vector3& v);
    /// get vector3 attribute from the entity
    const vector3& GetVector3(Attr::Vector3AttributeID attrId) const;
    /// set vector4 attribute on the entity
    void SetVector4(Attr::Vector4AttributeID attrId, const vector4& v);
    /// get vector4 attribute from the entity
    const vector4& GetVector4(Attr::Vector4AttributeID attrId) const;
    /// set matrix44 attribute on the entity
    void SetMatrix44(Attr::Matrix44AttributeID attrId, const matrix44& m);
    /// get matrix44 attribute from the entity
    const matrix44& GetMatrix44(Attr::Matrix44AttributeID attrid) const;

    /// add attributes found in `attrSet' if not already.
    void AddAttributeSet(AttrSet* attrSet);
    /// does this contain attribute set `attrSet'?
    bool HasAttributeSet(const AttrSet* attrSet) const;

    /// set Msg Dispatcher
    void SetDispatcher(Message::Dispatcher* disp);
    // get Msg Dispatcher
    Message::Dispatcher* GetDispatcher() const;

protected:
    /// setup the entity properties, called from OnActivate()
    virtual void SetupProperties();
    /// cleanup the entity properties, called from OnDeactivate()
    virtual void CleanupProperties();

private:
    /// call OnActivate() on all properties
    void ActivateProperties();
    /// call OnDeactivate() on all properties
    void DeactivateProperties();
    /// set entity pool
    void SetEntityPool(EntityPool p);

    friend class Managers::FactoryManager;

    Ptr<Message::Dispatcher> dispatcher;
    Ptr<Db::Entity> dbEntity;
    nArray<Ptr<Property> > properties;
    nArray<Ptr<AttrSet> > attrSets;
    EntityId uniqueId;
    EntityPool entityPool;

    static EntityId uniqueIdCounter;

    bool activated;
    bool isInOnActivate;
    bool isInOnDeactivate;
};

RegisterFactory(Entity);

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetDispatcher(Message::Dispatcher* disp)
{
    this->dispatcher = disp;
}

//------------------------------------------------------------------------------
/**
*/
inline
Message::Dispatcher*
Entity::GetDispatcher() const
{
    return this->dispatcher;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetAttr(const Db::Attribute& attr)
{
    this->dbEntity->SetAttr(attr);
}

//------------------------------------------------------------------------------
/**
*/
inline
const Db::Attribute&
Entity::GetAttr(Attr::AttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::HasAttr(Attr::AttributeID attrId) const
{
    return this->dbEntity->HasAttr(attrId);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<Db::Attribute>&
Entity::GetAttrs() const
{
    return this->dbEntity->GetAttrs();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetString(Attr::StringAttributeID attrId, const nString& s)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, s));
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
Entity::GetString(Attr::StringAttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetString();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetInt(Attr::IntAttributeID attrId, int i)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, i));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Entity::GetInt(Attr::IntAttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetFloat(Attr::FloatAttributeID attrId, float f)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, f));
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Entity::GetFloat(Attr::FloatAttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetBool(Attr::BoolAttributeID attrId, bool b)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, b));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::GetBool(Attr::BoolAttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetVector3(Attr::Vector3AttributeID attrId, const vector3& v)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, v));
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Entity::GetVector3(Attr::Vector3AttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetVector3();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetVector4(Attr::Vector4AttributeID attrId, const vector4& v)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, v));
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
Entity::GetVector4(Attr::Vector4AttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetVector4();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetMatrix44(Attr::Matrix44AttributeID attrId, const matrix44& m)
{
    this->dbEntity->SetAttr(Db::Attribute(attrId, m));
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Entity::GetMatrix44(Attr::Matrix44AttributeID attrId) const
{
    return this->dbEntity->GetAttr(attrId).GetMatrix44();
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Entity::GetUniqueId() const
{
    return this->uniqueId;
}

//------------------------------------------------------------------------------
/**
    Send a synchronous message to this entity.
*/
inline
void
Entity::SendSync(Message::Msg* msg)
{
    msg->SendSync(this->dispatcher);
}

//------------------------------------------------------------------------------
/**
    Send an asynchronous message to this entity.
*/
inline
void
Entity::SendAsync(Message::Msg* msg)
{
    n_assert(LivePool == this->entityPool);
    msg->SendAsync(this->dispatcher);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetEntityPool(EntityPool p)
{
    this->entityPool = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
Entity::EntityPool
Entity::GetEntityPool() const
{
    return this->entityPool;
}

} // namespace Game
//------------------------------------------------------------------------------
#endif
