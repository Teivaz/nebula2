//------------------------------------------------------------------------------
//  nsessionservercontext_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionservercontext.h"

static void n_getsessionguid(void* slf, nCmd* cmd);
static void n_gethostname(void* slf, nCmd* cmd);
static void n_getportname(void* slf, nCmd* cmd);
static void n_getserverattr(void* slf, nCmd* cmd);
static void n_getnumserverattrs(void* slf, nCmd* cmd);
static void n_getserverattrat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsessionservercontext
    @cppclass
    nSessionServerContext
    @superclass
    nroot
    @classinfo
    Represents a session server on the client side.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("s_getsessionguid_v",        'GSGD', n_getsessionguid);
    cl->AddCmd("s_gethostname_v",           'GHON', n_gethostname);
    cl->AddCmd("s_getportname_v",           'GPTN', n_getportname);
    cl->AddCmd("s_getserverattr_s",         'GSVA', n_getserverattr);
    cl->AddCmd("i_getnumserverattrs_v",     'GNSA', n_getnumserverattrs);
    cl->AddCmd("ss_getserverattrat_i",      'GSAA', n_getserverattrat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsessionguid
    @input
    v
    @output
    s(SessionGuid)
    @info
    Get the session guid.
*/
static void
n_getsessionguid(void* slf, nCmd* cmd)
{
    nSessionServerContext* self = (nSessionServerContext*) slf;
    cmd->Out()->SetS(self->GetSessionGuid());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gethostname
    @input
    v
    @output
    s(PortName)
    @info
    Get the port name string of the session server.
*/
static void
n_gethostname(void* slf, nCmd* cmd)
{
    nSessionServerContext* self = (nSessionServerContext*) slf;
    cmd->Out()->SetS(self->GetHostName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getportname
    @input
    v
    @output
    s(PortName)
    @info
    Get the port name string of the session server.
*/
static void
n_getportname(void* slf, nCmd* cmd)
{
    nSessionServerContext* self = (nSessionServerContext*) slf;
    cmd->Out()->SetS(self->GetPortName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getserverattr
    @input
    s(AttrName)
    @output
    s(AttrValue)
    @info
    Get a server attribute.
*/
static void
n_getserverattr(void* slf, nCmd* cmd)
{
    nSessionServerContext* self = (nSessionServerContext*) slf;
    cmd->Out()->SetS(self->GetServerAttr(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumserverattrs
    @input
    v
    @output
    i(NumServerAttrs)
    @info
    Get number of server attributes.
*/
static void
n_getnumserverattrs(void* slf, nCmd* cmd)
{
    nSessionServerContext* self = (nSessionServerContext*) slf;
    cmd->Out()->SetI(self->GetNumServerAttrs());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getserverattrat
    @input
    i(AttrIndex)
    @output
    s(AttrName), s(AttrValue)
    @info
    Get a server attribute by index.
*/
static void
n_getserverattrat(void* slf, nCmd* cmd)
{
    nSessionServerContext* self = (nSessionServerContext*) slf;
    const char* s0;
    const char* s1;
    self->GetServerAttrAt(cmd->In()->GetI(), s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

