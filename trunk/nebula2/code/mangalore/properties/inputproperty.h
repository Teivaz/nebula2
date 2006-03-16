#ifndef PROPERTIES_INPUTPROPERTY_H
#define PROPERTIES_INPUTPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::InputProperty

    An input property adds the ability to handle user input to an entity.
    If an InputProperty is attached to an entity it can become the input
    focus entity. Global input focus is managed by the Game::FocusManager
    singleton.

    If you want the concept of an input focus in your application you should
    derive your own input property classes from the InputProperty class,
    because then the FocusManager will be aware of it (otherwise it will
    just ignore the entity).
    
    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"
#include "attr/attributes.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareBool(InputFocus);
};

//------------------------------------------------------------------------------
namespace Properties
{
class InputProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(InputProperty);

public:
    /// constructor
    InputProperty();
    /// destructor
    virtual ~InputProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called when input focus is gained
    virtual void OnObtainFocus();
    /// called when input focus is lost
    virtual void OnLoseFocus();
    /// return true if currently has input focus
    virtual bool HasFocus() const;
    /// called on begin of frame
    virtual void OnBeginFrame();
};

RegisterFactory(InputProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif
