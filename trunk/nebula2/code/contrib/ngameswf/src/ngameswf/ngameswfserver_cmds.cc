//------------------------------------------------------------------------------
//  ngameswfserver_cmds.cc
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "ngameswf/ngameswfserver.h"
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
nGameSwfServer::SaveCmds (nPersistServer* ps)
{
    if (nRoot::SaveCmds (ps))
    {
        return true;
    }

    return false;
}
