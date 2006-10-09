//------------------------------------------------------------------------------
//  textbutton.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/textbutton.h"

namespace UI
{
ImplementRtti(UI::TextButton, UI::Button);
ImplementFactory(UI::TextButton);

// FIXME: if text is changed it must be reflected in the Nebula2
// text shape nodes...
} // namespace UI
