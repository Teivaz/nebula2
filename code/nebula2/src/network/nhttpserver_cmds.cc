//------------------------------------------------------------------------------
//  nhttpserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nhttpserver.h"

static void n_connect(void* slf, nCmd* cmd);
static void n_disconnect(void* slf, nCmd* cmd);
static void n_isconnected(void* slf, nCmd* cmd);
static void n_get(void* slf, nCmd* cmd);
static void n_post(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nhttpserver    
    @cppclass
    nHttpServer
    @superclass
    nroot
    @classinfo
    A Nebula2 server object which implements a simple http client.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_connect_s",     'CONN', n_connect);
    cl->AddCmd("v_disconnect_v",  'DSCN', n_disconnect);
    cl->AddCmd("b_isconnected_v", 'ISCN', n_isconnected);
    cl->AddCmd("bs_get_s",        'GET_', n_get);
    cl->AddCmd("bs_post_ss",      'POST', n_post);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    connect
    @input
    s(ServerName)
    @output
    b(Success)
    @info
    Connect to a http server.
*/
static void
n_connect(void* slf, nCmd* cmd)
{
    nHttpServer* self = (nHttpServer*) slf;
    cmd->Out()->SetB(self->Connect(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    disconnect
    @input
    v
    @output
    v
    @info
    Disconnect from http server.
*/
static void
n_disconnect(void* slf, nCmd* /*cmd*/)
{
    nHttpServer* self = (nHttpServer*) slf;
    self->Disconnect();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isconnected
    @input
    v
    @output
    b
    @info
    Return true if currently connected to a http server.
*/
static void
n_isconnected(void* slf, nCmd* cmd)
{
    nHttpServer* self = (nHttpServer*) slf;
    cmd->Out()->SetB(self->IsConnected());
}

//------------------------------------------------------------------------------
/**
    @cmd
    get
    @input
    s(HttpPath)
    @output
    b(Success), s(Result)
    @info
    Issue a GET request and return the result.
*/
static void
n_get(void* slf, nCmd* cmd)
{
    nHttpServer* self = (nHttpServer*) slf;
    nString result;
    bool retval = self->Get(cmd->In()->GetS(), result);
    cmd->Out()->SetB(retval);
    cmd->Out()->SetS(result.Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    post
    @input
    s(HttpPath), s(PostData)
    @output
    b(Success), s(Result)
    @info
    Issue a POST request and return the result.
*/
static void
n_post(void* slf, nCmd* cmd)
{
    nHttpServer* self = (nHttpServer*) slf;
    nString result;
    nString object = cmd->In()->GetS();
    nString post   = cmd->In()->GetS();
    bool retval = self->Post(object, post, result);
    cmd->Out()->SetB(retval);
    cmd->Out()->SetS(result.Get());
}



