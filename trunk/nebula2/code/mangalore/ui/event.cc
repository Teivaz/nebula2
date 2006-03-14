//------------------------------------------------------------------------------
//  ui/event.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/event.h"

namespace UI
{
ImplementRtti(UI::Event, Message::Msg);
ImplementFactory(UI::Event);
ImplementMsgId(Event);
};
