//------------------------------------------------------------------------------
//  nshadowserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shadow/nshadowserver.h"

static void n_setshowshadows(void* slf, nCmd* cmd);
static void n_getshowshadows(void* slf, nCmd* cmd);
static void n_setdebugshadows(void* slf, nCmd* cmd);
static void n_getdebugshadows(void* slf, nCmd* cmd);
static void n_setusezfail(void* slf, nCmd* cmd);
static void n_getusezfail(void* slf, nCmd* cmd);
static void n_setshadowcolor(void* slf, nCmd* cmd);
static void n_getshadowcolor(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadowserver
    @cppclass
    nShadowServer
    @superclass
    nroot
    @classinfo
    Server object of the shadow subsystem.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setshowshadows_b",    'SSHW', n_setshowshadows);
    cl->AddCmd("b_getshowshadows_v",    'GSHW', n_getshowshadows);
    cl->AddCmd("v_setdebugshadows_b",   'SDBW', n_setdebugshadows);
    cl->AddCmd("b_getdebugshadows_v",   'GDBW', n_getdebugshadows);
    cl->AddCmd("v_setusezfail_b",       'SUZF', n_setusezfail);
    cl->AddCmd("b_getusezfail_v",       'GUZF', n_getusezfail);
    cl->AddCmd("v_setshadowcolor_ffff", 'SSCL', n_setshadowcolor);
    cl->AddCmd("ffff_getshadowcolor_v", 'GSCL', n_getshadowcolor);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setshowshadows
    @input
    b(ShowShadows)
    @output
    v
    @info
    Enable/disable shadow rendering.
*/
static void
n_setshowshadows(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    self->SetShowShadows(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getshowshadows
    @input
    v
    @output
    b(ShowShadows)
    @info
    Get shadow rendering flag.
*/
static void
n_getshowshadows(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    cmd->Out()->SetB(self->GetShowShadows());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdebugshadows
    @input
    b(DebugShadows)
    @output
    v
    @info
    Enable/disable shadow debug rendering.
*/
static void
n_setdebugshadows(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    self->SetDebugShadows(cmd->In()->GetB());
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
    Get shadow rendering flag.
*/
static void
n_getdebugshadows(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    cmd->Out()->SetB(self->GetDebugShadows());
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
    nShadowServer* self = (nShadowServer*) slf;
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
    nShadowServer* self = (nShadowServer*) slf;
    cmd->Out()->SetB(self->GetUseZFail());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setshadowcolor
    @input
    ffff(ShadowColor)
    @output
    v
    @info
    Set the shadow color.
*/
static void
n_setshadowcolor(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetShadowColor(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getshadowcolor
    @input
    v
    @output
    ffff(ShadowColor)
    @info
    Get the shadow color.
*/
static void
n_getshadowcolor(void* slf, nCmd* cmd)
{
    nShadowServer* self = (nShadowServer*) slf;
    const vector4& v = self->GetShadowColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}





