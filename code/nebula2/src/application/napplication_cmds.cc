//------------------------------------------------------------------------------
//  napplication_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/napplication.h"

static void n_setcompanyname(void* slf, nCmd* cmd);
static void n_getcompanyname(void* slf, nCmd* cmd);
static void n_setappname(void* slf, nCmd* cmd);
static void n_getappname(void* slf, nCmd* cmd);
static void n_getcurrentstate(void* slf, nCmd* cmd);
static void n_setstate(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    napplication
    @cppclass
    nApplication
    @superclass
    nroot
    @classinfo
    Main class of the Nebula2 simple application framework.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcompanyname_s",  'SCPN', n_setcompanyname);
    cl->AddCmd("s_getcompanyname_v",  'GCPN', n_getcompanyname);
    cl->AddCmd("v_setappname_s",      'SAPN', n_setappname);
    cl->AddCmd("s_getappname_v",      'GAPN', n_getappname);
    cl->AddCmd("s_getcurrentstate_v", 'GCRS', n_getcurrentstate);
    cl->AddCmd("v_setstate_s",        'SETS', n_setstate);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcompanyname
    @input
    s(CompanyName)
    @output
    v
    @info
    Set the company name. This will be used together with the application
    name to identify the application in various places of the system
    (for instance the Windows registry, etc...).
*/
static void
n_setcompanyname(void* slf, nCmd* cmd)
{
    nApplication* self = (nApplication*) slf;
    self->SetCompanyName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcompanyname
    @input
    v
    @output
    s(CompanyName)
    @info
    Get the company name.
*/
static void
n_getcompanyname(void* slf, nCmd* cmd)
{
    nApplication* self = (nApplication*) slf;
    cmd->Out()->SetS(self->GetCompanyName().Get());
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
    Set the application name. This will be used together with the company
    name to identify the application in various places of the system
    (for instance the Windows registry, etc...).
*/
static void
n_setappname(void* slf, nCmd* cmd)
{
    nApplication* self = (nApplication*) slf;
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
    nApplication* self = (nApplication*) slf;
    cmd->Out()->SetS(self->GetAppName().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcurrentstate
    @input
    v
    @output
    s
    @info
    Retrieve name of the current state.

    04-Oct-04    kims    created
*/
static
void n_getcurrentstate(void* slf, nCmd* cmd)
{
    nApplication* self = (nApplication*) slf;
    cmd->Out()->SetS(self->GetCurrentState().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstate
    @input
    s
    @output
    v
    @info
    Specify the state with given string of the state name.

    04-Oct-04    kims    created
*/
static
void n_setstate(void* slf, nCmd* cmd)
{
    nApplication* self = (nApplication*) slf;
    nString stateName = cmd->In()->GetS();
    self->SetState(stateName);
}

