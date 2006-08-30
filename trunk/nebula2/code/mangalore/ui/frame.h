#ifndef UI_FRAME_H
#define UI_FRAME_H
//------------------------------------------------------------------------------
/**
    @class UI::Frame

    A frame is used to group UI elements together.

    (C) 2006 Radon Labs GmbH
*/
#include "ui/element.h"

//------------------------------------------------------------------------------
namespace UI
{
class Frame : public Element
{
    DeclareRtti;
    DeclareFactory(Frame);
public:
    /// constructor
    Frame();
    /// destructor
    virtual ~Frame();
};

};
//------------------------------------------------------------------------------
#endif