//------------------------------------------------------------------------------
//  nmrtsceneserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmrtsceneserver.h"

static void n_setcompositingenabled(void* slf, nCmd* cmd);
static void n_iscompositingenabled(void* slf, nCmd* cmd);
static void n_setsaturation(void* slf, nCmd* cmd);
static void n_getsaturation(void* slf, nCmd* cmd);
static void n_setbalance(void* slf, nCmd* cmd);
static void n_getbalance(void* slf, nCmd* cmd);
static void n_setluminance(void* slf, nCmd* cmd);
static void n_getluminance(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmrtsceneserver
    
    @superclass
    nsceneserver

    @classinfo
    The default scene server.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcompositingenabled_b", 'SCPE', n_setcompositingenabled);
    cl->AddCmd("b_istcompositingenabled_v", 'ICPE', n_iscompositingenabled);
    cl->AddCmd("v_setsaturation_f", 'SSAT', n_setsaturation);
    cl->AddCmd("f_getsaturation_v", 'GSAT', n_getsaturation);
    cl->AddCmd("v_setbalance_ffff", 'SBAL', n_setbalance);
    cl->AddCmd("ffff_getbalance_v", 'GBAL', n_getbalance);
    cl->AddCmd("v_setluminance_ffff", 'SLUM', n_setluminance);
    cl->AddCmd("ffff_getluminance_v", 'GLUM', n_getluminance);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcompositingenabled
    @input
    b(EnabledFlag)
    @output
    v
    @info
    Enable/disable frame compositing.
*/
static void
n_setcompositingenabled(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    self->SetCompositingEnabled(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    iscompositingenabled
    @input
    v
    @output
    b(EnabledFlag)
    @info
    Get compositing enabled flag.
*/
static void
n_iscompositingenabled(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    cmd->Out()->SetB(self->IsCompositingEnabled());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsaturation
    @input
    f(Saturation)
    @output
    v
    @info
    Set color saturation (usually 0..1).
*/
static void
n_setsaturation(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    self->SetSaturation(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsaturation
    @input
    v
    @output
    f(Saturation)
    @info
    Get color saturation (usually 0..1).
*/
static void
n_getsaturation(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    cmd->Out()->SetF(self->GetSaturation());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbalance
    @input
    ffff(ColorBalance)
    @output
    v
    @info
    Set the color balance value.
*/
static void
n_setbalance(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    static vector4 c;
    c.x = cmd->In()->GetF();
    c.y = cmd->In()->GetF();
    c.z = cmd->In()->GetF();
    c.w = cmd->In()->GetF();
    self->SetBalance(c);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getbalance
    @input
    v
    @output
    ffff(ColorBalance)
    @info
    Get the color balance value.
*/
static void
n_getbalance(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    const vector4& c = self->GetBalance();
    cmd->Out()->SetF(c.x);
    cmd->Out()->SetF(c.y);
    cmd->Out()->SetF(c.z);
    cmd->Out()->SetF(c.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setluminance
    @input
    ffff(LuminanceVector)
    @output
    v
    @info
    Set vector for luminance computation
    lum = c.r*l.r + c.g*l.g + c.b*l.b
*/
static void
n_setluminance(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    static vector4 l;
    l.x = cmd->In()->GetF();
    l.y = cmd->In()->GetF();
    l.z = cmd->In()->GetF();
    l.w = cmd->In()->GetF();
    self->SetLuminance(l);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getluminance
    @input
    v
    @output
    ffff(LuminanceVector)
    @info
    Get vector for luminance computation.
*/
static void
n_getluminance(void* slf, nCmd* cmd)
{
    nMRTSceneServer* self = (nMRTSceneServer*) slf;
    const vector4& l = self->GetLuminance();
    cmd->Out()->SetF(l.x);
    cmd->Out()->SetF(l.y);
    cmd->Out()->SetF(l.z);
    cmd->Out()->SetF(l.w);
}


