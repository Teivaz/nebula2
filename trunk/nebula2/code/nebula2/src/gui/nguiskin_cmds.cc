//------------------------------------------------------------------------------
//  nguiskin_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiskin.h"

static void n_settextureprefix(void* slf, nCmd* cmd);
static void n_gettextureprefix(void* slf, nCmd* cmd);
static void n_settexturepostfix(void* slf, nCmd* cmd);
static void n_gettexturepostfix(void* slf, nCmd* cmd);
static void n_setactivewindowcolor(void* slf, nCmd* cmd);
static void n_getactivewindowcolor(void* slf, nCmd* cmd);
static void n_setinactivewindowcolor(void* slf, nCmd* cmd);
static void n_getinactivewindowcolor(void* slf, nCmd* cmd);
static void n_setbuttontextcolor(void* slf, nCmd* cmd);
static void n_getbuttontextcolor(void* slf, nCmd* cmd);
static void n_settitletextcolor(void* slf, nCmd* cmd);
static void n_gettitletextcolor(void* slf, nCmd* cmd);
static void n_setlabeltextcolor(void* slf, nCmd* cmd);
static void n_getlabeltextcolor(void* slf, nCmd* cmd);
static void n_setwindowborder(void* slf, nCmd* cmd);
static void n_getwindowborder(void* slf, nCmd* cmd);
static void n_beginbrushes(void* slf, nCmd* cmd);
static void n_addbrush(void* slf, nCmd* cmd);
static void n_endbrushes(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiskin

    @cppclass
    nGuiSkin

    @superclass
    nroot

    @classinfo
    Holds a GUI skin description.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settextureprefix_s",          'STPR', n_settextureprefix);
    cl->AddCmd("s_gettextureprefix_v",          'GTPR', n_gettextureprefix);
    cl->AddCmd("v_settexturepostfix_s",         'STPS', n_settexturepostfix);
    cl->AddCmd("s_gettexturepostfix_v",         'GTPS', n_gettexturepostfix);
    cl->AddCmd("v_setactivewindowcolor_ffff",   'SAWC', n_setactivewindowcolor);
    cl->AddCmd("ffff_getactivewindowcolor_v",   'GAWC', n_getactivewindowcolor);
    cl->AddCmd("v_setinactivewindowcolor_ffff", 'SIAC', n_setinactivewindowcolor);
    cl->AddCmd("ffff_getinactivewindowcolor_v", 'GIAC', n_getinactivewindowcolor);
    cl->AddCmd("v_settitletextcolor_ffff",      'STTC', n_settitletextcolor);
    cl->AddCmd("ffff_gettitletextcolor_v",      'GTTC', n_gettitletextcolor);
    cl->AddCmd("v_setbuttontextcolor_ffff",     'SBTC', n_setbuttontextcolor);
    cl->AddCmd("ffff_getbuttontextcolor_v",     'GBTC', n_getbuttontextcolor);
    cl->AddCmd("v_setlabeltextcolor_ffff",      'SLTC', n_setlabeltextcolor);
    cl->AddCmd("ffff_getlabeltextcolor_v",      'GLTC', n_getlabeltextcolor);
    cl->AddCmd("v_setwindowborder_ffff",        'SWBS', n_setwindowborder);
    cl->AddCmd("ffff_getwindowborder_v",        'GWBS', n_getwindowborder);
    cl->AddCmd("v_beginbrushes_v",              'BGBR', n_beginbrushes);
    cl->AddCmd("v_addbrush_ssffffffff",         'ADBR', n_addbrush);
    cl->AddCmd("v_endbrushes_v",                'EDBR', n_endbrushes);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settextureprefix
    @input
    s(Prefix)
    @output
    v
    @info
    Set the path prefix for the texture names.
*/
static void
n_settextureprefix(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    self->SetTexturePrefix(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettextureprefix
    @input
    v
    @output
    s(Prefix)
    @info
    Get the path prefix for the texture names.
*/
static void
n_gettextureprefix(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    cmd->Out()->SetS(self->GetTexturePrefix());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settexturepostfix
    @input
    s(Postfix)
    @output
    v
    @info
    Set the path postfix for the texture names.
*/
static void
n_settexturepostfix(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    self->SetTexturePostfix(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettexturepostfix
    @input
    v
    @output
    s(Prefix)
    @info
    Get the path postfix for the texture names.
*/
static void
n_gettexturepostfix(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    cmd->Out()->SetS(self->GetTexturePostfix());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setactivewindowcolor
    @input
    ffff(Color)
    @output
    v
    @info
    Set the modulation color for active windows.
*/
static void
n_setactivewindowcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    vector4 c;
    c.x = cmd->In()->GetF();
    c.y = cmd->In()->GetF();
    c.z = cmd->In()->GetF();
    c.w = cmd->In()->GetF();
    self->SetActiveWindowColor(c);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getactivewindowcolor
    @input
    v
    @output
    ffff(Color)
    @info
    Get the modulation color for active windows.
*/
static void
n_getactivewindowcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    const vector4& c = self->GetActiveWindowColor();
    cmd->Out()->SetF(c.x);
    cmd->Out()->SetF(c.y);
    cmd->Out()->SetF(c.z);
    cmd->Out()->SetF(c.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setinactivewindowcolor
    @input
    ffff(Color)
    @output
    v
    @info
    Set the modulation color for inactive windows.
*/
static void
n_setinactivewindowcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    vector4 c;
    c.x = cmd->In()->GetF();
    c.y = cmd->In()->GetF();
    c.z = cmd->In()->GetF();
    c.w = cmd->In()->GetF();
    self->SetInactiveWindowColor(c);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getinactivewindowcolor
    @input
    v
    @output
    ffff(Color)
    @info
    Get the modulation color for inactive windows.
*/
static void
n_getinactivewindowcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    const vector4& c = self->GetInactiveWindowColor();
    cmd->Out()->SetF(c.x);
    cmd->Out()->SetF(c.y);
    cmd->Out()->SetF(c.z);
    cmd->Out()->SetF(c.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settitletextcolor
    @input
    ffff(Color)
    @output
    v
    @info
    Set the default text color for the window titles.
*/
static void
n_settitletextcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    vector4 c;
    c.x = cmd->In()->GetF();
    c.y = cmd->In()->GetF();
    c.z = cmd->In()->GetF();
    c.w = cmd->In()->GetF();
    self->SetTitleTextColor(c);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettitletextcolor
    @input
    v
    @output
    ffff(Color)
    @info
    Get the text color for window titles.
*/
static void
n_gettitletextcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    const vector4& c = self->GetTitleTextColor();
    cmd->Out()->SetF(c.x);
    cmd->Out()->SetF(c.y);
    cmd->Out()->SetF(c.z);
    cmd->Out()->SetF(c.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setbuttontextcolor
    @input
    ffff(Color)
    @output
    v
    @info
    Set the default button text color.
*/
static void
n_setbuttontextcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    vector4 c;
    c.x = cmd->In()->GetF();
    c.y = cmd->In()->GetF();
    c.z = cmd->In()->GetF();
    c.w = cmd->In()->GetF();
    self->SetButtonTextColor(c);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getbuttontextcolor
    @input
    v
    @output
    ffff(Color)
    @info
    Get the text color for buttons.
*/
static void
n_getbuttontextcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    const vector4& c = self->GetButtonTextColor();
    cmd->Out()->SetF(c.x);
    cmd->Out()->SetF(c.y);
    cmd->Out()->SetF(c.z);
    cmd->Out()->SetF(c.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setlabeltextcolor
    @input
    ffff(Color)
    @output
    v
    @info
    Set the default label text color.
*/
static void
n_setlabeltextcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    vector4 c;
    c.x = cmd->In()->GetF();
    c.y = cmd->In()->GetF();
    c.z = cmd->In()->GetF();
    c.w = cmd->In()->GetF();
    self->SetLabelTextColor(c);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getlabeltextcolor
    @input
    v
    @output
    ffff(Color)
    @info
    Get the text color for labels.
*/
static void
n_getlabeltextcolor(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    const vector4& c = self->GetLabelTextColor();
    cmd->Out()->SetF(c.x);
    cmd->Out()->SetF(c.y);
    cmd->Out()->SetF(c.z);
    cmd->Out()->SetF(c.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setwindowborder
    @input
    ffff(BorderRect: left, top, right, bottom)
    @output
    v
    @info
    Set the layout border size around the window's client area.
*/
static void
n_setwindowborder(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    rectangle r;
    r.v0.x = cmd->In()->GetF();
    r.v0.y = cmd->In()->GetF();
    r.v1.x = cmd->In()->GetF();
    r.v1.y = cmd->In()->GetF();
    self->SetWindowBorder(r);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getwindowbordersize
    @input
    v
    @output
    ffff(BorderRect)
    @info
    Get the layout border size around the window's client area.
*/
static void
n_getwindowborder(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    const rectangle& r = self->GetWindowBorder();
    cmd->Out()->SetF(r.v0.x);
    cmd->Out()->SetF(r.v0.y);
    cmd->Out()->SetF(r.v1.x);
    cmd->Out()->SetF(r.v1.y);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    beginbrushes
    @input
    v
    @output
    v
    @info
    Begin adding brushes. This will clear the current brush definitions.
*/
static void
n_beginbrushes(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    self->BeginBrushes();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addbrush
    @input
    s(BrushName), s(TexturePath), ffff(x,y,w,h), ffff(rgba)
    @output
    v
    @info
    Defines a brush by its name, a path to a texture, and a rectangle in the
    texture. The rectangle is given in absolute texel coordinates (0..texWidth).
*/
static void
n_addbrush(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    vector2 uvPos;
    vector2 uvSize;
    vector4 color;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    uvPos.x  = cmd->In()->GetF();
    uvPos.y  = cmd->In()->GetF();
    uvSize.x = cmd->In()->GetF();
    uvSize.y = cmd->In()->GetF();
    color.x  = cmd->In()->GetF();
    color.y  = cmd->In()->GetF();
    color.z  = cmd->In()->GetF();
    color.w  = cmd->In()->GetF();
    self->AddBrush(s0, s1, uvPos, uvSize, color);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    endbrushes
    @input
    v
    @output
    v
    @info
    Finish adding brushes.
*/
static void
n_endbrushes(void* slf, nCmd* cmd)
{
    nGuiSkin* self = (nGuiSkin*) slf;
    self->EndBrushes();
}
