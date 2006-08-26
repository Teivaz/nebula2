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
static void n_setscissorrect(void* slf, nCmd* cmd);
static void n_getscissorrect(void* slf, nCmd* cmd);
static void n_setcursorvisibility(void* slf, nCmd* cmd);
static void n_setmousecursor(void* slf, nCmd* cmd);
static void n_seticon(void* slf, nCmd* cmd);
static void n_setgamma(void* slf, nCmd* cmd);
static void n_getgamma(void* slf, nCmd* cmd);
static void n_setbrightness(void* slf, nCmd* cmd);
static void n_getbrightness(void* slf, nCmd* cmd);
static void n_setcontrast(void* slf, nCmd* cmd);
static void n_getcontrast(void* slf, nCmd* cmd);
static void n_adjustgamma(void* slf, nCmd* cmd);
static void n_restoregamma(void* slf, nCmd* cmd);
static void n_setskipmsgloop(void* slf, nCmd* cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    ngfxserver2

    @cppclass
    nGfxServer2

    @superclass
    nroot

    @classinfo
    Generation 2 gfx server. Completely vertex/pixel shader based.

    23-Aug-04    kims    added setgamma, setbrightness, setcontrast, adjustgamma
                         and restoregamma command.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setdisplaymode_ssiiiib",   'SDMD', n_setdisplaymode);
    cl->AddCmd("ssiiiib_getdisplaymode_v",   'GDMD', n_getdisplaymode);
    cl->AddCmd("b_opendisplay_v",            'ODSP', n_opendisplay);
    cl->AddCmd("v_closedisplay_v",           'CDSP', n_closedisplay);
    cl->AddCmd("s_getfeatureset_v",          'GFTS', n_getfeatureset);
    cl->AddCmd("v_savescreenshot_s",         'SSCS', n_savescreenshot);
    cl->AddCmd("v_setscissorrect_ffff",      'SSCR', n_setscissorrect);
    cl->AddCmd("ffff_getscissorrect_v",      'GSCR', n_getscissorrect);
    cl->AddCmd("v_setcursorvisibility_s",   'SCVS', n_setcursorvisibility);
    cl->AddCmd("v_setmousecursor_sii",      'SMCS', n_setmousecursor);
    cl->AddCmd("v_seticon_s",               'SICO', n_seticon);
    cl->AddCmd("v_setgamma_f",              'SETG', n_setgamma);
    cl->AddCmd("f_getgamma_v",              'GETG', n_getgamma);
    cl->AddCmd("v_setbrightness_f",         'SETB', n_setbrightness);
    cl->AddCmd("f_getbrightness_v",         'GETB', n_getbrightness);
    cl->AddCmd("v_setcontrast_f",           'SETC', n_setcontrast);
    cl->AddCmd("f_getcontrast_v",           'GETC', n_getcontrast);
    cl->AddCmd("v_adjustgamma_v",           'ADJG', n_adjustgamma);
    cl->AddCmd("v_restoregamma_v",          'RESG', n_restoregamma);
    cl->AddCmd("v_setskipmsgloop_b",        'SSML', n_setskipmsgloop);
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

    self->SetDisplayMode(nDisplayMode2(title, type, x, y, w, h, vsync, true, "Icon")); // true - dialog box mode
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
    cmd->Out()->SetS(mode.GetWindowTitle().Get());
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
n_closedisplay(void* slf, nCmd* /*cmd*/)
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
    s(FeatureSet = dx7, dx8, dx8sb, dx9, dx9flt, invalid)
    @info
    Get the feature set implemented by the graphics card.
*/
static void
n_getfeatureset(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    nGfxServer2::FeatureSet feat = self->GetFeatureSet();
    cmd->Out()->SetS(nGfxServer2::FeatureSetToString(feat));
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
    A JPG file will be created.
*/
static void
n_savescreenshot(void *slf, nCmd *cmd)
{
    nGfxServer2 *self = (nGfxServer2*) slf;
    self->SaveScreenshot(cmd->In()->GetS(), nTexture2::JPG);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setcursorvisibility

    @input
    s('none', 'system', or 'custom')

    @output
    v

    @info
    Set whether no cursor, a standard or a custom mouse
    cursor will be displayed.
*/
static void
n_setcursorvisibility(void *slf, nCmd *cmd)
{
    nGfxServer2 *self = (nGfxServer2*) slf;
    const char *str = cmd->In()->GetS();
    nGfxServer2::CursorVisibility visibility;
    if (0 == strcmp(str, "none"))        visibility = nGfxServer2::None;
    else if (0 == strcmp(str, "system")) visibility = nGfxServer2::System;
    else if (0 == strcmp(str, "custom")) visibility = nGfxServer2::Custom;
    else
    {
        n_error("setcursorvisibility: invalid string '%s'\n", str);
        return;
    }
    self->SetCursorVisibility( visibility );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmousecursor

    @input
    s (texture resource name), ii (hotspot x, y)

    @output
    v

    @info
    Defines a custom mouse cursor.
*/
static void
n_setmousecursor(void *slf, nCmd *cmd)
{
    nGfxServer2 *self = (nGfxServer2*) slf;
    nMouseCursor cursor;
    cursor.SetFilename(cmd->In()->GetS());
    cursor.SetHotspotX(cmd->In()->GetI());
    cursor.SetHotspotY(cmd->In()->GetI());
    self->SetMouseCursor( cursor );
}


//------------------------------------------------------------------------------
/**
    @cmd
    seticon
    @input
    v
    @output
    s(icon resource name)
    @info
    Sets the window's icon.
    This must happen outside opendisplay/closedisplay.
*/
static void
n_seticon(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    nDisplayMode2 mode = self->GetDisplayMode();
    mode.SetIcon(cmd->In()->GetS());
    self->SetDisplayMode(mode);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setscissorrect
    @input
    f(x0), f(y0), f(x1), f(y1)
    @output
    v
    @info
    Define the scissor rectangle (topleft is (0.0f, 0.0f), bottomright
    is (1.0f, 1.0f)). Note that scissoring must be enabled externally
    in a shader!
*/
static void
n_setscissorrect(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    vector2 v0, v1;
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v1.x = cmd->In()->GetF();
    v1.y = cmd->In()->GetF();
    self->SetScissorRect(rectangle(v0, v1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscissorrect
    @input
    v
    @output
    f(x0), f(y0), f(x1), f(y1)
    @info
    Returns the current scissor rect.
*/
static void
n_getscissorrect(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    const rectangle& r = self->GetScissorRect();
    cmd->Out()->SetF(r.v0.x);
    cmd->Out()->SetF(r.v0.y);
    cmd->Out()->SetF(r.v1.x);
    cmd->Out()->SetF(r.v1.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgamma
    @input
    f
    @output
    v
    @info
    Set gamma value (adjustgamma must be called for the change to take effect)

    23-Aug-04    kims    created
*/
static
void n_setgamma(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    self->SetGamma(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getgamma
    @input
    v
    @output
    f
    @info
    Get current gamma value (reflects the last call to setgamma, which may not
    correspond to the actual screen gamma if adjustgamma has not been called).

    8-Sep-04    rafael    created
*/
static
void n_getgamma(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    cmd->Out()->SetF(self->GetGamma());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setbrightness
    @input
    f
    @output
    v
    @info
    Set brightness value (adjustgamma must be called for the change to take effect)

    23-Aug-04    kims    created
*/
static
void n_setbrightness(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    self->SetBrightness(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getbrightness
    @input
    v
    @output
    f
    @info
    Get current brightness (reflects last call to setbrightness, which may not
    correspond to the actual screen brightness if adjustgamma has not been called).

    8-Sep-04    rafael    created
*/
static
void n_getbrightness(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    cmd->Out()->SetF(self->GetBrightness());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setcontrast
    @input
    f
    @output
    v
    @info
    Set contrast value (adjustgamma must be called for the change to take effect)

    23-Aug-04    kims    created
*/
static void
n_setcontrast(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    self->SetContrast(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getcontrast
    @input
    v
    @output
    f
    @info
    Get current contrast (reflects last call to setcontrast, which may not
    correspond to the actual screen contrast if adjustgamma has not been called).

    8-Sep-04    rafael    created
*/
static
void n_getcontrast(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    cmd->Out()->SetF(self->GetContrast());
}
//------------------------------------------------------------------------------
/**
    @cmd
    adjustgamma
    @input
    v
    @output
    v
    @info
    Commits the last gamma, contrast, and brightness values set,
    so that they are actually visible on the screen.
    23-Aug-04    kims    created
*/
static void
n_adjustgamma(void* slf, nCmd* /*cmd*/)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    self->AdjustGamma();
}

//------------------------------------------------------------------------------
/**
    @cmd
    restoregamma
    @input
    v
    @output
    v
    @info
    Resets screen gamma to default values.

    23-Aug-04    kims    created
*/
static void
n_restoregamma(void* slf, nCmd* /*cmd*/)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    self->RestoreGamma();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskipmsgloop
    @input
    b(SkipMsgLoop)
    @output
    v
    @info
    Set whether or not the window handler should skip its message loop. This
    is required when embedding Nebula into an application that provides its
    own event loop, such as using wxWidgets.
*/
static void
n_setskipmsgloop(void* slf, nCmd* cmd)
{
    nGfxServer2* self = (nGfxServer2*) slf;
    bool skipMsgLoop = cmd->In()->GetB();
    self->SetSkipMsgLoop(skipMsgLoop);
}

