//------------------------------------------------------------------------------
//  script/command.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "script/command.h"

namespace Script
{
ImplementRtti(Script::Command, Foundation::RefCounted);
ImplementFactory(Script::Command);

//------------------------------------------------------------------------------
/**
*/
Command::Command()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Command::~Command()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
Command::Execute(const Util::CmdLineArgs& args)
{
    // override in subclass
    return true;
}

} // namespace Script
