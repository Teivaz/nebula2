//------------------------------------------------------------------------------
//  nsqlserver_cmds.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "sql/nsqlserver.h"

static void n_newdatabase(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsqlserver
    @cppclass
    nSqlServer
    @superclass
    nroot
    @classinfo
    Server class of the SQL subsystem. Used to access databases (gain
    access to a database through the newdatabase command).
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("o_newdatabase_s", 'NWDB', n_newdatabase);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    newdatabase
    @input
    s(resName)
    @output
    o(dbObject)
    @info
    Gain access to new or existing database. resName should be the "filename"
    which the actual database needs to gain access to the database. It is
    also used to share the database within Nebula2.
*/
static void
n_newdatabase(void* slf, nCmd* cmd)
{
    nSqlServer* self = (nSqlServer*) slf;
    cmd->Out()->SetO(self->NewDatabase(cmd->In()->GetS()));
}
