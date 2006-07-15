//------------------------------------------------------------------------------
//  ngopenalserver_cmds.cc
//  (C) 2004 Bang, Chang Kyu.
//------------------------------------------------------------------------------
#include "nopenal/nopenalserver.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds ();
    cl->EndCmds ();
}


//------------------------------------------------------------------------------
bool 
nOpenALServer::SaveCmds (nPersistServer* ps)
{
    if (nRoot::SaveCmds (ps))
    {
        nCmd* cmd = NULL;
        return true;
    }
    return false;
}
