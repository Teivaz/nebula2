#ifndef PROPERTIES_VIDEOCAMERAPROPERTY2_H
#define PROPERTIES_VIDEOCAMERAPROPERTY2_H
//------------------------------------------------------------------------------
/**
    A manually controlled camera property which implements different
    control models.
    
    (C) 2005 Radon Labs GmbH
*/
#include "properties/cameraproperty.h"
#include "tools/nmayacamcontrol.h"
#include "attr/attributes.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareFloat(FilmAspectRatio);
    DeclareFloat(NearClipPlane);
    DeclareFloat(FarClipPlane);
    DeclareMatrix44(ProjectionMatrix);
    DeclareString(AnimPath);
};

//------------------------------------------------------------------------------
namespace Properties
{
class VideoCameraProperty2 : public CameraProperty
{
    DeclareRtti;
    DeclareFactory(VideoCameraProperty2);

public:
    /// constructor
    VideoCameraProperty2();
    /// destructor
    virtual ~VideoCameraProperty2();
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called when camera focus is obtained
    virtual void OnObtainFocus();
    /// called before rendering happens
    virtual void OnRender();

private:
    nMayaCamControl mayaCamera;
};

RegisterFactory(VideoCameraProperty2);

}; // namespace Properties
//------------------------------------------------------------------------------
#endif 