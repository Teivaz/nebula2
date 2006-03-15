//------------------------------------------------------------------------------
//  script/echo.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "script/echo.h"
#include "script/ntclserver.h"


namespace Script
{
ImplementRtti(Script::Echo, Foundation::RefCounted);
ImplementFactory(Script::Echo);

Echo* Echo::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
Echo::Echo()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Echo::~Echo()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
int
Echo::Execute(const Util::CmdLineArgs& args)
{
    // FIXME: Fill me
    Tcl_SetResult(((nTclServer *)(Foundation::Server::Instance()->GetScriptServer()))->GetInterp(), 
        "Dies ist ein Test !!!!!", TCL_STATIC);
    return true;
}

//------------------------------------------------------------------------------
/** get the name of the command
*/
nString
Echo::GetName() const
{
    return "echo";
}

}; // namespace Script
