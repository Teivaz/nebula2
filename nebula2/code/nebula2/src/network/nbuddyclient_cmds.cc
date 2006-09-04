//------------------------------------------------------------------------------
//  nbuddyclient_cmds.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nbuddyclient.h"

static void n_createuser(void* slf, nCmd* cmd);
static void n_login(void* slf, nCmd* cmd);
static void n_message(void* slf, nCmd* cmd);
static void n_addbuddy(void* slf, nCmd* cmd);
static void n_getbuddylist(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nbuddyclient
    @superclass
    nroot
    @classinfo
    The network buddyclient object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_createuser_ss",   'CRTU', n_createuser);
    cl->AddCmd("v_login_ss",        'LOGN', n_login);
    cl->AddCmd("v_message_ss",      'MSSG', n_message);
    cl->AddCmd("v_addbuddy_s",      'ADDB', n_addbuddy);
    cl->AddCmd("v_getbuddylist_v",    'GETB', n_getbuddylist);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    createuser
    @input
    s(username)
    @output
    v
    @info
    creates a new user
*/
static void
n_createuser(void* slf, nCmd* cmd)
{
    nBuddyClient* self = (nBuddyClient*) slf;

    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->CreateUser(s0,s1);
}



static void
n_login(void* slf, nCmd* cmd)
{
    nBuddyClient* self = (nBuddyClient*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->Login(s0,s1);
}

static void
n_message(void* slf, nCmd* cmd)
{
    nBuddyClient* self = (nBuddyClient*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SendMessage(s0,s1);
}

static void
n_addbuddy(void* slf, nCmd* cmd)
{
    nBuddyClient* self = (nBuddyClient*) slf;
    const char* s0 = cmd->In()->GetS();
    self->AddBuddy(s0);
}


static void
n_getbuddylist(void* slf, nCmd* cmd)
{
    nBuddyClient* self = (nBuddyClient*) slf;
    self->GetBuddylist();
}
