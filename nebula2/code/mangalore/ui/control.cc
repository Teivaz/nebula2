//------------------------------------------------------------------------------
//  ui/control.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/control.h"

namespace UI
{
ImplementRtti(UI::Control, UI::Element);
ImplementFactory(UI::Control);

//------------------------------------------------------------------------------
/**
*/
Control::Control() :
    enabled(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Control::~Control()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Parse GUI attributes on create.
*/
void
Control::OnCreate(Element* parent)
{
    Element::OnCreate(parent);
    this->SetEnabled(this->gfxNode->GetBoolAttr("rlGuiEnabled"));
}

}; // namespace UI
