#ifndef PROPERTIES_CAMERAPROPERTY_H
#define PROPERTIES_CAMERAPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::CameraProperty

    A camera property adds the ability to manipulate the camera to an entity.
    Please note that more advanced camera properties should always be
    derived from the class camera property if camera focus handling is desired,
    since the FocusManager will only work on game entities which have
    a CameraProperty (or a subclass) attached.

    It is completely ok though to handle camera manipulation in a property
    not derived from CameraProperty, but please be aware that the
    FocusManager will ignore those.

    The camera property will generally

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"
#include "attr/attributes.h"
#include "vfx/shakeeffecthelper.h"

//------------------------------------------------------------------------------
namespace Attr
{
    // camera specific attributes
    DeclareBool(CameraFocus);
    DeclareFloat(FieldOfView);
};

namespace Properties
{
class CameraProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(CameraProperty);

public:
    /// constructor
    CameraProperty();
    /// destructor
    virtual ~CameraProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called when camera focus is obtained
    virtual void OnObtainFocus();
    /// called when camera focus is lost
    virtual void OnLoseFocus();
    /// called before rendering happens
    virtual void OnRender();
    /// return true if currently has camera focus
    virtual bool HasFocus() const;

protected:
    VFX::ShakeEffectHelper shakeEffectHelper;
};

RegisterFactory(CameraProperty);

} // namespace Properties
//------------------------------------------------------------------------------
#endif
