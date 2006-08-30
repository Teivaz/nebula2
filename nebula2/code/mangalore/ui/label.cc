//------------------------------------------------------------------------------
//  ui/label.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/label.h"

namespace UI
{
ImplementRtti(UI::Label, UI::Element);
ImplementFactory(UI::Label);

//------------------------------------------------------------------------------
/**
*/
Label::Label()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Label::~Label()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called before rendering happens. This updates the visibility of our
    Nebula2 visual nodes.
*/
void
Label::OnRender()
{
    // render only if element is visible
    this->gfxNode->SetActive(this->IsVisible());

    Element::OnRender();
}

} // namespace UI
