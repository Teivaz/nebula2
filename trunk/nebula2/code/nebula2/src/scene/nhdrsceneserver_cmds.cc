//------------------------------------------------------------------------------
//  nhdrsceneserver_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nhdrsceneserver.h"

static void n_sethuecolor(void* slf, nCmd* cmd);
static void n_gethuecolor(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nhdrsceneserver
    
    @superclass
    nsceneserver

    @classinfo
    A scene server for high dynamic range rendering.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_sethuecolor_ffff", 'SHCL', n_sethuecolor);
    cl->AddCmd("ffff_gethuecolor_v", 'GHCL', n_gethuecolor);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    sethuecolor
    @input
    ffff(HueColor)
    @output
    v
    @info
    Set the hue color.
*/
static void
n_sethuecolor(void* slf, nCmd* cmd)
{
    nHdrSceneServer* self = (nHdrSceneServer*) slf;
    static vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetHueColor(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gethuecolor
    @input
    v
    @output
    ffff(HueColor)
    @info
    Get the hue color.
*/
static void
n_gethuecolor(void* slf, nCmd* cmd)
{
    nHdrSceneServer* self = (nHdrSceneServer*) slf;
    const vector4& v = self->GetHueColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}






