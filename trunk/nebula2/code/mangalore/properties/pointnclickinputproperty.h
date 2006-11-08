#ifndef PROPERTIES_POINTNCLICKINPUTPROPERTY_H
#define PROPERTIES_POINTNCLICKINPUTPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::PointNClickInputProperty

    An basic point'n'click third person input property which creates Goto
    messages when clicking into the environment.

    (C) 2005 Radon Labs GmbH
*/
#include "properties/inputproperty.h"

//------------------------------------------------------------------------------
namespace Properties
{
class PointNClickInputProperty : public InputProperty
{
    DeclareRtti;
	DeclareFactory(PointNClickInputProperty);

public:
    /// constructor
    PointNClickInputProperty();
    /// destructor
    virtual ~PointNClickInputProperty();
    /// called on begin of frame
    virtual void OnBeginFrame();

protected:
    /// handle the default action on left mouse click
    virtual void OnLmbDown();
    /// handle continuous left mouse press
    virtual void OnLmbPressed();
    /// handle a right mouse button click
    virtual void OnRmbDown();
    /// handle camera orbit movement
    virtual void OnMmbPressed();
    /// handle camera zoom in
    virtual void OnCameraZoomIn();
    /// handle camera zoom out
    virtual void OnCameraZoomOut();

private:
    /// send a MoveGoto message to the world intersection point
    void SendMoveGoto();

    nTime moveGotoTime;
};

RegisterFactory(PointNClickInputProperty);

} // namespace Properties
//------------------------------------------------------------------------------
#endif
