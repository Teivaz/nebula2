//------------------------------------------------------------------------------
//  ngfxserver2_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/ngfxserver2.h"

static void n_setdisplaymode(void* slf, nCmd* cmd);
static void n_getdisplaymode(void* slf, nCmd* cmd);
static void n_opendisplay(void* slf, nCmd* cmd);
static void n_closedisplay(void* slf, nCmd* cmd);
static void n_getfeatureset(void* slf, nCmd* cmd);
static void n_savescreenshot(void* slf, nCmd* cmd);


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
    cl->AddCmd("v_setdisplaymode_ssiiiib",  'SDMD', n_setdisplaymode);
    cl->AddCmd("siiiib_getdisplaymode_v",   'GDMD', n_getdisplaymode);
    cl->AddCmd("b_opendisplay_v",      'ODSP', n_opendisplay);
    cl->AddCmd("v_closedisplay_v",     'CDSP', n_closedisplay);
    cl->AddCmd("s_getfeatureset_v",         'GFTS', n_getfeatureset);
    cl->AddCmd("v_savescreenshot_s",        'SSCS', n_savescreenshot);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdisplaymode
    @input
    s(WindowTitle), s(Type=windowed,fullscreen|alwaysontop), i(XPos), i(YPos), i(Width), i(Height), b(VSync)
    @output
    v
    @info
    Set a new display mode. This must happen outside opendisplay/closedisplay.
*/
static void
n_setdisplaymode(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
	const char* title = cmd->In()->GetS();
    nDisplayMode2::Type type = nDisplayMode2::StringToType(cmd->In()->GetS());
    int x = cmd->In()->GetI();
    int y = cmd->In()->GetI();
    int w = cmd->In()->GetI();
    int h = cmd->In()->GetI();
    bool vsync = cmd->In()->GetB();

    self->SetDisplayMode(nDisplayMode2(title, type, x, y, w, h, vsync));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdisplaymode
    @input
    v
    @output
    s(WindowTitle), s(Type=windowed,fullscreen|alwaysontop), i(XPos), i(YPos), i(Width), i(Height), b(VSync)
    @info
    Get the current display mode.
*/
static void
n_getdisplaymode(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    const nDisplayMode2& mode = self->GetDisplayMode();
    cmd->Out()->SetS(mode.GetWindowTitle());
    cmd->Out()->SetS(nDisplayMode2::TypeToString(mode.GetType()));
    cmd->Out()->SetI(mode.GetXPos());
    cmd->Out()->SetI(mode.GetYPos());
    cmd->Out()->SetI(mode.GetWidth());
    cmd->Out()->SetI(mode.GetHeight());
    cmd->Out()->SetB(mode.GetVerticalSync());
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

//------------------------------------------------------------------------------
/**
    @cmd
    getfeatureset
    @input
    v
    @output
    s(FeatureSet = dx7, dx8, dx8sb, dx9, invalid)
    @info
    Get the feature set implemented by the graphics card.
*/
static void
n_getfeatureset(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    nGfxServer2::FeatureSet feat = self->GetFeatureSet();
    switch (feat)
    {
        case nGfxServer2::DX7:                      cmd->Out()->SetS("dx7"); break;
        case nGfxServer2::DX8:                      cmd->Out()->SetS("dx8"); break;
        case nGfxServer2::DX8SB:                    cmd->Out()->SetS("dx8sb"); break;
        case nGfxServer2::DX9:                      cmd->Out()->SetS("dx9"); break;
        default:                                    cmd->Out()->SetS("invalid"); break;
    }
}

    @output
    v

    @info
    Generate a screenshot under the given filename.
    A 24 bpp BMP file will be created.
*/
static void
n_savescreenshot(void *slf, nCmd *cmd)
{
    nGfxServer2 *self = (nGfxServer2*) slf;
    cmd->Out()->SetB(self->SaveScreenshot(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    savescreenshot

    @input
    s(Filename)

    @output
    v

    @info
    Save a screenshot to the provided filename.
*/
static void
n_savescreenshot(void *slf, nCmd *cmd)
{
    nGfxServer2 *self = (nGfxServer2*) slf;
    self->SaveScreenshot(cmd->In()->GetS());
}


