//------------------------------------------------------------------------------
//  nsessionclientcontext_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionclientcontext.h"

static void n_getclientattr(void* slf, nCmd* cmd);
static void n_getnumclientattrs(void* slf, nCmd* cmd);
static void n_getclientattrat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsessionclientcontext
    @cppclass
    nSessionClientContext
    @superclass
    nroot
    @classinfo
    Represents a session client on the server side.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("s_getclientattr_s",     'GCLA', n_getclientattr);
    cl->AddCmd("i_getnumclientattrs_v", 'GNCA', n_getnumclientattrs);
    cl->AddCmd("ss_getclientattrat_i",  'GCAA', n_getclientattrat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientattr
    @input
    s(AttrName)
    @output
    s(AttrValue)
    @info
    Get a client attribute. Client attributes originate on joined clients
    and are communicated to the server.
*/
static void
n_getclientattr(void* slf, nCmd* cmd)
{
    nSessionClientContext* self = (nSessionClientContext*) slf;
    cmd->Out()->SetS(self->GetClientAttr(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumclientattrs
    @input
    v
    @output
    i(NumClientAttrs)
    @info
    Get number of client attributes.
*/
static void
n_getnumclientattrs(void* slf, nCmd* cmd)
{
    nSessionClientContext* self = (nSessionClientContext*) slf;
    cmd->Out()->SetI(self->GetNumClientAttrs());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientattrat
    @input
    i(AttrIndex)
    @output
    s(AttrName), s(AttrValue)
    @info
    Get a client attribute by index.
*/
static void
n_getclientattrat(void* slf, nCmd* cmd)
{
    nSessionClientContext* self = (nSessionClientContext*) slf;
    const char* s0;
    const char* s1;
    self->GetClientAttrAt(cmd->In()->GetI(), s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

