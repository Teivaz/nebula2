#define N_IMPLEMENTS nGfxServer2
//------------------------------------------------------------------------------
//  ngfxserver2_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/ngfxserver2.h"

static void n_setdisplaymode(void* slf, nCmd* cmd);
static void n_getdisplaymode(void* slf, nCmd* cmd);
static void n_opendisplay(void* slf, nCmd* cmd);
static void n_closedisplay(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ngfxserver2

    @superclass
    nroot

    @classinfo
    Generation 2 gfx server. Completely vertex/pixel shader based.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setdisplaymode_sii", 'SDMD', n_setdisplaymode);
    cl->AddCmd("sii_getdisplaymode_v", 'GDMD', n_getdisplaymode);
    cl->AddCmd("b_opendisplay_v",      'ODSP', n_opendisplay);
    cl->AddCmd("v_closedisplay_v",     'CDSP', n_closedisplay);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdisplaymode
    @input
    s(Type=windowed,fullscreen,child), i(Width), i(Height)
    @output
    v
    @info
    Set a new display mode. This must happen outside opendisplay/closedisplay.
*/
static void
n_setdisplaymode(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    nDisplayMode2::Type t = nDisplayMode2::StringToType(cmd->In()->GetS());
    int w = cmd->In()->GetI();
    int h = cmd->In()->GetI();

    self->SetDisplayMode(nDisplayMode2(t, w, h));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdisplaymode
    @input
    v
    @output
    s(Type=windowed,fullscreen,child), i(Width), i(Height)
    @info
    Get the current display mode.
*/
static void
n_getdisplaymode(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    int w = self->GetDisplayMode().GetWidth();
    int h = self->GetDisplayMode().GetHeight();
    const char* t = nDisplayMode2::TypeToString(self->GetDisplayMode().GetType());
    
    cmd->Out()->SetS(t);
    cmd->Out()->SetI(w);
    cmd->Out()->SetI(h);
}

//------------------------------------------------------------------------------
/**
    @cmd
    opendisplay
    @input
    v
    @output
    b(Success)
    @info
    Open the display.
*/
static void
n_opendisplay(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    cmd->Out()->SetB(self->OpenDisplay());
}

//------------------------------------------------------------------------------
/**
    @cmd
    closedisplay
    @input
    v
    @output
    v
    @info
    Close the display.
*/
static void
n_closedisplay(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    self->CloseDisplay();
}
