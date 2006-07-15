//------------------------------------------------------------------------------
//  nsessionclient_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nsessionclient.h"

static void n_setappname(void* slf, nCmd* cmd);
static void n_getappname(void* slf, nCmd* cmd);
static void n_setappversion(void* slf, nCmd* cmd);
static void n_getappversion(void* slf, nCmd* cmd);
static void n_setclientattr(void* slf, nCmd* cmd);
static void n_getclientattr(void* slf, nCmd* cmd);
static void n_getnumclientattrs(void* slf, nCmd* cmd);
static void n_getclientattrat(void* slf, nCmd* cmd);
static void n_getnumservers(void* slf, nCmd* cmd);
static void n_getserverat(void* slf, nCmd* cmd);
static void n_getjoinedserver(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_joinsession(void* slf, nCmd* cmd);
static void n_leavesession(void* slf, nCmd* cmd);
static void n_isjoined(void* slf, nCmd* cmd);
static void n_isjoinaccepted(void* slf, nCmd* cmd);
static void n_isjoindenied(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsessionclient
    @cppclass
    nSessionClient
    @superclass
    nroot
    @classinfo
    The network session client class.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setappname_s",        'SAPN', n_setappname);
    cl->AddCmd("s_getappname_v",        'GAPN', n_getappname);
    cl->AddCmd("v_setappversion_s",     'SAPV', n_setappversion);
    cl->AddCmd("s_getappversion_v",     'GAPV', n_getappversion);
    cl->AddCmd("v_setclientattr_ss",    'SCLA', n_setclientattr);
    cl->AddCmd("s_getclientattr_s",     'GCLA', n_getclientattr);
    cl->AddCmd("i_getnumclientattrs_v", 'GNCA', n_getnumclientattrs);
    cl->AddCmd("ss_getclientattrat_i",  'GCAA', n_getclientattrat);
    cl->AddCmd("i_getnumservers_v",     'GNMS', n_getnumservers);
    cl->AddCmd("o_getserverat_i",       'GSAT', n_getserverat);
    cl->AddCmd("o_getjoinedserver_v",   'GJSR', n_getjoinedserver);
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->AddCmd("b_joinsession_s",       'JNSS', n_joinsession);
    cl->AddCmd("b_leavesession_v",      'LVSS', n_leavesession);
    cl->AddCmd("b_isjoined_v",          'ISJN', n_isjoined);
    cl->AddCmd("b_isjoinaccepted_v",    'ISJA', n_isjoinaccepted);
    cl->AddCmd("b_isjoindenied_v",      'ISJD', n_isjoindenied);
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
    nSessionClient* self = (nSessionClient*) slf;
    self->SetAppName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getappname
    @input
    v
    @output
    s(AppName)
    @info
    Get the application name.
*/
static void
n_getappname(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
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
    nSessionClient* self = (nSessionClient*) slf;
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
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetS(self->GetAppVersion());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setclientattr
    @input
    s(AttrName), s(AttrValue)
    @output
    v
    @info
    Set a client attribute.
*/
static void
n_setclientattr(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetClientAttr(s0, s1);
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
    Get a client attribute.
*/
static void
n_getclientattr(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
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
    nSessionClient* self = (nSessionClient*) slf;
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
    nSessionClient* self = (nSessionClient*) slf;
    const char* s0;
    const char* s1;
    self->GetClientAttrAt(cmd->In()->GetI(), s0, s1);
    cmd->Out()->SetS(s0);
    cmd->Out()->SetS(s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumservers
    @input
    v
    @output
    i(NumDiscoveredServers)
    @info
    Get number of discovered servers.
*/
static void
n_getnumservers(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetI(self->GetNumServers());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getserverat
    @input
    i(Index)
    @output
    o(ServerContextObject)
    @info
    Get session server object at given index.
*/
static void
n_getserverat(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetO(self->GetServerAt(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjoinedserver
    @input
    v
    @output
    o(ServerContextObject)
    @info
    Get reference to joined server.
*/
static void
n_getjoinedserver(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetO(self->GetJoinedServer());
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
    Open the session client. This will start discovering open sessions
    on the network.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
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
    Close the session client.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nSessionClient* self = (nSessionClient*) slf;
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
    Check if session client is currently open.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    joinsession
    @input
    s(SessionGuid)
    @output
    b(PartialSuccessCode)
    @info
    Send a join request to the session server identified by the session's
    guid. The success code only indicates, whether a session of that
    guid could be found, it does NOT indicate whether the join was
    successful. The session server will reply a join accepted or denied
    message at some later time.
*/
static void
n_joinsession(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetB(self->JoinSession(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    leavesession
    @input
    v
    @output
    b
    @info
    Leave the currently joined session.
*/
static void
n_leavesession(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetB(self->LeaveSession());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isjoined
    @input
    v
    @output
    b(JoinedFlag)
    @info
    Return true if currently joined to a session.
*/
static void
n_isjoined(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetB(self->IsJoined());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isjoinaccepted
    @input
    v
    @output
    b(JoinAcceptedFlag)
    @info
    Return true if the server had accepted the join session request.
*/
static void
n_isjoinaccepted(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetB(self->IsJoinAccepted());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isjoindenied
    @input
    v
    @output
    b(JoinDeniedFlag)
    @info
    Return true if the server had denied the join session request.
*/
static void
n_isjoindenied(void* slf, nCmd* cmd)
{
    nSessionClient* self = (nSessionClient*) slf;
    cmd->Out()->SetB(self->IsJoinDenied());
}
