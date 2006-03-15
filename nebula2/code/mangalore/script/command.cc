//------------------------------------------------------------------------------
//  script/command.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "script/command.h"


namespace Script
{
ImplementRtti(Script::Command, Foundation::RefCounted);
ImplementFactory(Script::Command);

Command* Command::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Command::Command()
{
    //n_assert(0 == Command::Singleton);
    //Command::Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Command::~Command()
{
    //n_assert(0 != Command::Singleton);
    //Command::Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
int
Command::Execute(const Util::CmdLineArgs& args)
{
    // FIXME: Fill me
    return true;
}

//------------------------------------------------------------------------------
/** get the name of the command
*/
nString
Command::GetName() const
{
    return "NONE";
}

}; // namespace Script
