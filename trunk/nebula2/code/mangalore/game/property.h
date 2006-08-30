#ifndef GAME_PROPERTY_H
#define GAME_PROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Game::Property

    Properties are attached to game entities to add specific functionality
    or behaviours to the entity. For instance, you add a GraphicsProperty
    if the entity should be able render itself, or you add an AudioProperty
    if the entity should be able to emit sound. Properties are derived from
    the Message::Port class and as such may receive and handle
    messages but they are not required to do so.

    (C) 2006 RadonLabs GmbH
*/
#include "message/port.h"
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace Game
{
class Entity;

class Property : public Message::Port
{
    DeclareRtti;
public:
    /// constructor
    Property();
    /// destructor
    virtual ~Property() = 0;

    /// return mask of entity pool flags where this property is active
    virtual int GetActiveEntityPools() const;
    /// returns whether this property should be attached to entities in the given state
    virtual bool IsActiveInState(const nString& state) const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// return true if property is currently active
    bool IsActive() const;
    /// called from within Entity::Load() after attributes are loaded
    virtual void OnLoad();
    /// called from within Entity::OnStart() after OnLoad when the complete world exist
    virtual void OnStart();
    /// called from within Entity::Save() before attributes are saved back to database
    virtual void OnSave();
    /// called at the start of a frame
    virtual void OnBeginFrame();
    /// called before movement happens
    virtual void OnMoveBefore();
    /// called after movement has happened
    virtual void OnMoveAfter();
    /// called before rendering happens
    virtual void OnRender();
    /// called before rendering happens
    virtual void OnRenderDebug();
    /// return true if message is accepted by a property
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);
    /// Entity this is attached to.
    Entity* GetEntity() const;
    /// Is this attached to an entity?
    bool HasEntity() const;

protected:
    friend class Entity;
    /// Set entity, this is attached to, to `v'.
    void SetEntity(Entity* v);
    /// Remove entity.
    void ClearEntity();

    Entity* entity;
    bool active;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
Property::IsActive() const
{
    return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline
Entity*
Property::GetEntity() const
{
    n_assert(HasEntity());
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Property::HasEntity() const
{
    return this->entity != 0;
}

} // namespace Game
//------------------------------------------------------------------------------
#endif
