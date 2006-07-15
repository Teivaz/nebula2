//------------------------------------------------------------------------------
//  nshadowserver2_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow2/nshadowserver2.h"

static void n_setenableshadows(void* slf, nCmd* cmd);
static void n_getenableshadows(void* slf, nCmd* cmd);
static void n_setusezfail(void* slf, nCmd* cmd);
static void n_getusezfail(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadowserver
    @cppclass
    nShadowServer2
    @superclass
    nroot
    @classinfo
    Server object of the shadow subsystem.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setenableshadows_b",  'SSHW', n_setenableshadows);
    cl->AddCmd("b_getenableshadows_v",  'GSHW', n_getenableshadows);
    cl->AddCmd("v_setusezfail_b",       'SUZF', n_setusezfail);
    cl->AddCmd("b_getusezfail_v",       'GUZF', n_getusezfail);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setenableshadows
    @input
    b(EnableShadows)
    @output
    v
    @info
    Enable/disable shadow rendering.
*/
static void
n_setenableshadows(void* slf, nCmd* cmd)
{
    nShadowServer2* self = (nShadowServer2*) slf;
    self->SetEnableShadows(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getenableshadows
    @input
    v
    @output
    b(EnableShadows)
    @info
    Get shadow rendering flag.
*/
static void
n_getenableshadows(void* slf, nCmd* cmd)
{
    nShadowServer2* self = (nShadowServer2*) slf;
    cmd->Out()->SetB(self->GetEnableShadows());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setusezfail
    @input
    b
    @output
    v
    @info
    Set use zFail rendering.
*/
static void
n_setusezfail(void* slf, nCmd* cmd)
{
    nShadowServer2* self = (nShadowServer2*) slf;
    self->SetUseZFail(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdebugshadows
    @input
    v
    @output
    b(DebugShadows)
    @info
    Set use zFail rendering.
*/
static void
n_getusezfail(void* slf, nCmd* cmd)
{
    nShadowServer2* self = (nShadowServer2*) slf;
    cmd->Out()->SetB(self->GetUseZFail());
}

