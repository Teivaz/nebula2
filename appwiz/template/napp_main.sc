//----------------------------------------------------------------------------
//   %(classNameL)s_main.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "%(subDirL)s/%(classNameL)s.h"
#include "%(subDirL)s/%(appStateNameL)s.h"

nNebulaScriptClass(%(className)s, "napplication")

//----------------------------------------------------------------------------
/**
*/
%(className)s::%(className)s()
{
}

//----------------------------------------------------------------------------
/**
*/
%(className)s::~%(className)s()
{
}

//----------------------------------------------------------------------------
/**
*/
bool %(className)s::Open()
{
    if (!nApplication::Open())
    {
        return false;
    }

    //TODO: Add code for opening the application.

    // create edit state and specifies it to application.
    this->%(appStateObjName)s = (%(appStateName)s*)this->CreateState("%(appStateNameL)s", "%(appStateObjName)s");
    this->SetState("%(appStateObjName)s");

    return true;
}

//----------------------------------------------------------------------------
/**
*/
void %(className)s::Close()
{
    //TODO: Add code for closing the application.

    nApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer*
%(className)s::CreateScriptServer()
{
    return (nScriptServer*) kernelServer->New("%(scriptServerName)s", "/sys/servers/script");
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
