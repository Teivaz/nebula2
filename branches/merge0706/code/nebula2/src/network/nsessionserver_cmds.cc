//------------------------------------------------------------------------------
//  nsessionserver_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionserver.h"

static void n_setappname(void* slf, nCmd* cmd);
static void n_getappname(void* slf, nCmd* cmd);
static void n_setappversion(void* slf, nCmd* cmd);
static void n_getappversion(void* slf, nCmd* cmd);
static void n_setmaxnumclients(void* slf, nCmd* cmd);
static void n_getmaxnumclients(void* slf, nCmd* cmd);
static void n_setserverattr(void* slf, nCmd* cmd);
static void n_getserverattr(void* slf, nCmd* cmd);
static void n_getnumserverattrs(void* slf, nCmd* cmd);
static void n_getserverattrat(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_getnumclients(void* slf, nCmd* cmd);
static void n_getclientat(void* slf, nCmd* cmd);
static void n_kickclient(void* slf, nCmd* cmd);
static void n_start(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsessionserver
    @cppclass
    nSessionServer
    @superclass
    nroot
    @classinfo
    The network session server class.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setappname_s",        'SAPN', n_setappname);
    cl->AddCmd("s_getappname_v",        'GAPN', n_getappname);
    cl->AddCmd("v_setappversion_s",     'SAPV', n_setappversion);
    cl->AddCmd("s_getappversion_v",     'GAPV', n_getappversion);
    cl->AddCmd("v_setmaxnumclients_i",  'SMNC', n_setmaxnumclients);
    cl->AddCmd("i_getmaxnumclients_v",  'GMNC', n_getmaxnumclients);
    cl->AddCmd("v_setserverattr_ss",    'SSVA', n_setserverattr);
    cl->AddCmd("s_getserverattr_s",     'GSVA', n_getserverattr);
    cl->AddCmd("i_getnumserverattrs_v", 'GNSA', n_getnumserverattrs);
    cl->AddCmd("ss_getserverattrat_i",  'GSAA', n_getserverattrat);
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->AddCmd("i_getnumclients_v",     'GNCL', n_getnumclients);
    cl->AddCmd("o_getclientat_i",       'GCLA', n_getclientat);
    cl->AddCmd("b_kickclient_i",        'KCKC', n_kickclient);
    cl->AddCmd("b_start_v",             'STRT', n_start);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setappname
    @input
    s(AppName)
    @output
    v
    @info
    Set the application name. Will be used together with the AppVersion
    to identify sessions.
*/
static void
n_setappname(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    self->SetAppName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getappname
    @input
    @output
    s(AppName)
    @info
    Get the application name.
*/
static void
n_getappname(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetS(self->GetAppName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setappversion
    @input
    s(AppVersion)
    @output
    v
    @info
    Set the version string. Will be used together with the AppName
    to identify sessions.
*/
static void
n_setappversion(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    self->SetAppVersion(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getappversion
    @input
    v
    @output
    s(AppVersion)
    @info
    Get the application version string.
*/
static void
n_getappversion(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetS(self->GetAppVersion());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmaxnumclients
    @input
    i(MaxNumClients)
    @output
    v
    @info
    Set max number of clients which may join a session.    
*/
static void
n_setmaxnumclients(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    self->SetMaxNumClients(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaxnumclients
    @input
    v
    @output
    i(MaxNumClients)
    @info
    Get max number of clients which may join a session.
*/
static void
n_getmaxnumclients(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetI(self->GetMaxNumClients());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setserverattr
    @input
    s(AttrName), s(AttrValue)
    @output
    v
    @info
    Set a server attribute. Server attributes will be distributed to
    joined and potential clients on requests.
*/
static void
n_setserverattr(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetServerAttr(s0, s1);
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
    nSessionServer* self = (nSessionServer*) slf;
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
    nSessionServer* self = (nSessionServer*) slf;
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
    nSessionServer* self = (nSessionServer*) slf;
    const char* s0;
    const char* s1;
    self->GetServerAttrAt(cmd->In()->GetI(), s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
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
    Open the session. This will make the session visible in the network.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
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
    Close the session. The session will no longer be visible in the network.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nSessionServer* self = (nSessionServer*) slf;
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
    Check if session is currently open.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetB(self->IsOpen());
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
    Get the number of clients which are currently selected.
*/
static void
n_getnumclients(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetI(self->GetNumClients());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclientat
    @input
    i(ClientIndex)
    @output
    o(ClientContextObject)
    @info
    Get client context object at index. The client object context contains
    information about a client.
*/
static void
n_getclientat(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetO(self->GetClientAt(cmd->In()->GetI()));
}
//------------------------------------------------------------------------------
/**
    @cmd
    kickclient
    @input
    i(ClientIndex)
    @output
    b(Success)
    @info
    Kick a client from the session.
*/
static void
n_kickclient(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetB(self->KickClient(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    start
    @input
    v
    @output
    b(Success)
    @info
    Start the actual game session.
*/
static void
n_start(void* slf, nCmd* cmd)
{
    nSessionServer* self = (nSessionServer*) slf;
    cmd->Out()->SetB(self->Start());
}
