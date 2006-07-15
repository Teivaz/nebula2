//------------------------------------------------------------------------------
//  nnetclient_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nnetclient.h"

static void n_setclientguid(void* slf, nCmd* cmd);
static void n_getclientguid(void* slf, nCmd* cmd);
static void n_setserverhostname(void* slf, nCmd* cmd);
static void n_getserverhostname(void* slf, nCmd* cmd);
static void n_setserverportname(void* slf, nCmd* cmd);
static void n_getserverportname(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_getclientstatus(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnetclient
    @cppclass
    nNetClient
    @superclass
    nroot
    @classinfo
    The network client object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setclientguid_s",     'SCLG', n_setclientguid);
    cl->AddCmd("s_getclientguid_v",     'GCLG', n_getclientguid);
    cl->AddCmd("v_setserverhostname_s", 'SSVH', n_setserverhostname);
    cl->AddCmd("s_getserverhostname_v", 'GSVH', n_getserverhostname);
    cl->AddCmd("v_setserverportname_s", 'SSVP', n_setserverportname);
    cl->AddCmd("s_getserverportname_v", 'GSVP', n_getserverportname);
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->AddCmd("s_getclientstatus_v",   'GCST', n_getclientstatus);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setclientguid
    @input
    s(PortName)
    @output
    v
    @info
    Set the client guid which is used for authentification.
*/
static void
n_setclientguid(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    self->SetClientGuid(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientguid
    @input
    v
    @output
    s(PortName)
    @info
    Get the client guid which is used for authentification.
*/
static void
n_getclientguid(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    cmd->Out()->SetS(self->GetClientGuid());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setserverhostname
    @input
    s(ServerHostName)
    @output
    v
    @info
    Set the server's host name.
*/
static void
n_setserverhostname(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    self->SetServerHostName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getserverhostname
    @input
    v
    @output
    s(ServerHostName)
    @info
    Get the server's host name.
*/
static void
n_getserverhostname(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    cmd->Out()->SetS(self->GetServerHostName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setserverportname
    @input
    s(ServerPortName)
    @output
    v
    @info
    Set the server's port name.
*/
static void
n_setserverportname(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    self->SetServerPortName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getserverportname
    @input
    v
    @output
    s(ServerPortName)
    @info
    Get the server's port name.
*/
static void
n_getserverportname(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    cmd->Out()->SetS(self->GetServerPortName());
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
    Open the client.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
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
    Close the client.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nNetClient* self = (nNetClient*) slf;
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
    Return the open status of the client.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientstatus
    @input
    v
    @output
    s(ClientStatus)
    @info
    Get the current client status. This is one of 
    invalid, connecting, connected, joinaccepted, joindenied, started,
    timeout.
*/
static void
n_getclientstatus(void* slf, nCmd* cmd)
{
    nNetClient* self = (nNetClient*) slf;
    switch (self->GetClientStatus())
    {
        case nNetClient::Connecting:    cmd->Out()->SetS("connecting"); break;
        case nNetClient::JoinAccepted:  cmd->Out()->SetS("joinaccepted"); break;
        case nNetClient::JoinDenied:    cmd->Out()->SetS("joindenied"); break;
        case nNetClient::Started:       cmd->Out()->SetS("started"); break;
        case nNetClient::Timeout:       cmd->Out()->SetS("timeout"); break;
        default:                        cmd->Out()->SetS("invalid"); break;
    }
}
