//------------------------------------------------------------------------------
//  nguitextlabel_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextlabel.h"

static void n_setfont(void* slf, nCmd* cmd);
static void n_getfont(void* slf, nCmd* cmd);
static void n_setalignment(void* slf, nCmd* cmd);
static void n_getalignment(void* slf, nCmd* cmd);
static void n_setcolor(void* slf, nCmd* cmd);
static void n_getcolor(void* slf, nCmd* cmd);
static void n_setshadowcolor(void* slf, nCmd* cmd);
static void n_getshadowcolor(void* slf, nCmd* cmd);
static void n_setshadowoffset(void* slf, nCmd* cmd);
static void n_getshadowoffset(void* slf, nCmd* cmd);
static void n_settext(void* slf, nCmd* cmd);
static void n_gettext(void* slf, nCmd* cmd);
static void n_setint(void* slf, nCmd* cmd);
static void n_getint(void* slf, nCmd* cmd);
static void n_setborder(void* slf, nCmd* cmd);
static void n_getborder(void* slf, nCmd* cmd);
static void n_setclipping(void* slf, nCmd* cmd);
static void n_getclipping(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguitextlabel

    @cppclass
    nGuiTextLabel

    @superclass
    nguilabel

    @classinfo
    A gui label which can render ASCII text with optional background bitmap.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfont_s",           'SFNT', n_setfont);
    cl->AddCmd("s_getfont_v",           'GFNT', n_getfont);
    cl->AddCmd("v_setalignment_s",      'SAGN', n_setalignment);
    cl->AddCmd("s_getalignment_v",      'GAGN', n_getalignment);
    cl->AddCmd("v_setcolor_ffff",       'SCLR', n_setcolor);
    cl->AddCmd("ffff_getcolor_v",       'GCLR', n_getcolor);
    cl->AddCmd("v_setshadowcolor_ffff", 'SSCL', n_setshadowcolor);
    cl->AddCmd("ffff_getshadowcolor_v", 'GSCL', n_getshadowcolor);
    cl->AddCmd("v_setshadowoffset_ff",  'SSOF', n_setshadowoffset);
    cl->AddCmd("ff_getshadowoffset_v",  'GSOF', n_getshadowoffset);
    cl->AddCmd("v_settext_s",           'STXT', n_settext);
    cl->AddCmd("s_gettext_v",           'GTXT', n_gettext);
    cl->AddCmd("v_setint_i",            'SINT', n_setint);
    cl->AddCmd("i_getint_v",            'GINT', n_getint);
    cl->AddCmd("v_setborder_ff",        'SBRD', n_setborder);
    cl->AddCmd("ff_getborder_v",        'GBRD', n_getborder);
    cl->AddCmd("v_setclipping_b",       'SCLP', n_setclipping);
    cl->AddCmd("b_getclipping_v",       'GCLP', n_getclipping);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setfont
    @input
    s(FontName)
    @output
    v
    @info
    Set the font name. The font must have been registered with the
    gui server with the nguiserver.addfont command.
*/
static void
n_setfont(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    self->SetFont(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getfont
    @input
    v
    @output
    s(FontName)
    @info
    Get the font name.
*/
static void
n_getfont(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    cmd->Out()->SetS(self->GetFont());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setalignment
    @input
    s(Alignment = left | center | right)
    @output
    v
    @info
    Set the (horizontal) text alignment) in the text widget.
*/
static void
n_setalignment(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    const char* str = cmd->In()->GetS();
    nGuiTextLabel::Alignment align;
    if (strcmp("left", str) == 0)        align = nGuiTextLabel::Left;
    else if (strcmp("right", str) == 0)  align = nGuiTextLabel::Right;
    else if (strcmp("center", str) == 0) align = nGuiTextLabel::Center;
    else 
    {
        align = nGuiTextLabel::Center;
        n_error("nguitextlabel.setalign: Invalid align string '%s'", str);
    }
    self->SetAlignment(align);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getalignment
    @input
    v
    @output
    s(Alignment = left | center | right)
    @info
    Get the (horizontal) text alignment) in the text widget.
*/
static void
n_getalignment(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    switch (self->GetAlignment())
    {
        case nGuiTextLabel::Left:   cmd->Out()->SetS("left"); break;
        case nGuiTextLabel::Center: cmd->Out()->SetS("center"); break;
        default:                    cmd->Out()->SetS("right"); break;
    }
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setcolor
    @input
    f(Red), f(Green), f(Blue), f(Alpha)
    @output
    v
    @info
    Set the text color.
*/
static void
n_setcolor(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetColor(vector4(f0, f1, f2, f3));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getcolor
    @input
    v
    @output
    f(Red), f(Green), f(Blue), f(Alpha)
    @info
    Get the text color.
*/
static void
n_getcolor(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    const vector4& v = self->GetColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setshadowcolor
    @input
    f(Red), f(Green), f(Blue), f(Alpha)
    @output
    v
    @info
    Set the text shadow color.
*/
static void
n_setshadowcolor(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetShadowColor(vector4(f0, f1, f2, f3));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getshadowcolor
    @input
    v
    @output
    f(Red), f(Green), f(Blue), f(Alpha)
    @info
    Get the text shadow color.
*/
static void
n_getshadowcolor(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    const vector4& v = self->GetShadowColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setshadowoffset
    @input
    f(XOffset), f(YOffset)
    @output
    v
    @info
    Set the text shadow x/y offset in screen space units. The default
    value is 0.005f.
*/
static void
n_setshadowoffset(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    self->SetShadowOffset(vector2(f0, f1));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getshadowoffset
    @input
    v
    @output
    f(XOffset), f(YOffset)
    @info
    Get the text shadow x/y offset.
*/
static void
n_getshadowoffset(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    const vector2& v = self->GetShadowOffset();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settext
    @input
    s(TextString)
    @output
    v
    @info
    Set the label text as string.
*/
static void
n_settext(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    self->SetText(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettext
    @input
    v
    @output
    s(TextString)
    @info
    Get the label text as string.
*/
static void
n_gettext(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    cmd->Out()->SetS(self->GetText());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setint
    @input
    i(IntegerValue)
    @output
    v
    @info
    Set the label text as an integer.
*/
static void
n_setint(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    self->SetInt(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getint
    @input
    v
    @output
    i(IntegerValue)
    @info
    Get the label text as integer.
*/
static void
n_getint(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    cmd->Out()->SetI(self->GetInt());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setborder
    @input
    f(BorderX), f(BorderY)
    @output
    v
    @info
    Set the label's border size.
*/
static void
n_setborder(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    vector2 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    self->SetBorder(v);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getborder
    @input
    v
    @output
    f(BorderX), f(BorderY)
    @info
    Get the label's border size.
*/
static void
n_getborder(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    const vector2& v = self->GetBorder();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setclipping
    @input
    b(ClippingFlag)
    @output
    v
    @info
    Enable/disable text clipping. Default is enabled.
*/
static void
n_setclipping(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    self->SetClipping(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getclipping
    @input
    v
    @output
    b(ClippingFlag)
    @info
    Get the text clipping flag.
*/
static void
n_getclipping(void* slf, nCmd* cmd)
{
    nGuiTextLabel* self = (nGuiTextLabel*) slf;
    cmd->Out()->SetB(self->GetClipping());
}
