//------------------------------------------------------------------------------
//  audio/listener.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/listener.h"

namespace Audio
{
ImplementRtti(Audio::Listener, Foundation::RefCounted);
ImplementFactory(Audio::Listener);

//------------------------------------------------------------------------------
/**
*/
Listener::Listener()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Listener::~Listener()
{
    // empty
}

} // namespace Audio