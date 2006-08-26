//------------------------------------------------------------------------------
//  nnetserver_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nnetserver.h"

static void n_setportname(void* slf, nCmd* cmd);
static void n_getportname(void* slf, nCmd* cmd);
static void n_beginclients(void* slf, nCmd* cmd);
static void n_setclientguid(void* slf, nCmd* cmd);
static void n_endclients(void* slf, nCmd* cmd);
static void n_getnumclients(void* slf, nCmd* cmd);
static void n_getclientguidat(void* slf, nCmd* cmd);
static void n_getclientstatusat(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_isstarted(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnetserver
    @cppclass
    nNetServer
    @superclass
    nroot
    @classinfo
    The network server object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setportname_s",       'SAPN', n_setportname);
    cl->AddCmd("s_getportname_v",       'GAPN', n_getportname);
    cl->AddCmd("v_beginclients_i",      'BCLT', n_beginclients);
    cl->AddCmd("v_setclientguid_s",     'SCLG', n_setclientguid);
    cl->AddCmd("v_endclients_v",        'ECLT', n_endclients);
    cl->AddCmd("i_getnumclients_v",     'GNCL', n_getnumclients);
    cl->AddCmd("s_getclientguidat_i",   'GCGA', n_getclientguidat);
    cl->AddCmd("s_getclientstatusat_i", 'GCSA', n_getclientstatusat);
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->AddCmd("b_isstarted_v",         'ISST', n_isstarted);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setportname
    @input
    s(PortName)
    @output
    v
    @info
    Set the communication port name name.
*/
static void
n_setportname(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    self->SetPortName(cmd->In()->GetS());
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
    Get the communication port name.
*/
static void
n_getportname(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    cmd->Out()->SetS(self->GetPortName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginclients
    @input
    i(NumClients)
    @output
    v
    @info
    Begin configuration clients which are allowed to connect. Only clients
    added between beginclients and endclients will be allowed to join
    the session.
*/
static void
n_beginclients(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    self->BeginClients(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setclientguid
    @input
    i(ClientIndex), s(ClientGuid)
    @output
    v
    @info
    Set the guid for a client. The guid is used to authenticate a client
    which wants to connect to the session.
*/
static void
n_setclientguid(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    self->SetClientGuid(i0, s0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endclients
    @input
    v
    @output
    v
    @info
    Finish configuring clients.
*/
static void
n_endclients(void* slf, nCmd* /*cmd*/)
{
    nNetServer* self = (nNetServer*) slf;
    self->EndClients();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumclients
    @input
    v
    @output
    i(NumClients)
    @info
    Return number of configured (NOT JOINED!) clients.
*/
static void
n_getnumclients(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    cmd->Out()->SetI(self->GetNumClients());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientguidat
    @input
    i(ClientIndex)
    @output
    s(ClientGuid)
    @info
    Get the client guid at index.
*/
static void
n_getclientguidat(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    cmd->Out()->SetS(self->GetClientGuidAt(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientstatusat
    @input
    i(ClientIndex)
    @output
    s(ClientStatus)
    @info
    Returns the current client status.
*/
static void
n_getclientstatusat(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    nNetServer::ClientStatus clientStatus = self->GetClientStatusAt(cmd->In()->GetI());
    switch (clientStatus)
    {
        case nNetServer::Waiting:   cmd->Out()->SetS("waiting"); break;
        case nNetServer::Connected: cmd->Out()->SetS("connected"); break;
        case nNetServer::Timeout:   cmd->Out()->SetS("timeout"); break;
        default:                    cmd->Out()->SetS("invalid"); break;
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    open
    @input
    v
    @output
    b(Success)
    @info
    Open the server. The server expects the configured clients to connect,
    once all configured clients have connected, the isstarted command
    will return true.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    cmd->Out()->SetB(self->Open());
}

//------------------------------------------------------------------------------
/**
    @cmd
    close
    @input
    v
    @output
    v
    @info
    Close the server. The running session will be closed.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nNetServer* self = (nNetServer*) slf;
    self->Close();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isopen
    @input
    v
    @output
    b(OpenFlag)
    @info
    Return true if the server is open. This is the case between the
    open and close command.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isstarted
    @input
    v
    @output
    b(StartedFlag)
    @info
    Return true if the server is in the started state. This is the case when
    all configured clients have connected.
*/
static void
n_isstarted(void* slf, nCmd* cmd)
{
    nNetServer* self = (nNetServer*) slf;
    cmd->Out()->SetB(self->IsStarted());
}
