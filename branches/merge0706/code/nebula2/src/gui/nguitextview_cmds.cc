//------------------------------------------------------------------------------
//  nguitextview_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextview.h"

static void n_setfont(void* slf, nCmd* cmd);
static void n_getfont(void* slf, nCmd* cmd);
static void n_settextcolor(void* slf, nCmd* cmd);
static void n_gettextcolor(void* slf, nCmd* cmd);
static void n_setborder(void* slf, nCmd* cmd);
static void n_getborder(void* slf, nCmd* cmd);
static void n_beginappend(void* slf, nCmd* cmd);
static void n_appendline(void* slf, nCmd* cmd);
static void n_endappend(void* slf, nCmd* cmd);
static void n_getnumvisiblelines(void* slf, nCmd* cmd);
static void n_setselectionenabled(void* slf, nCmd* cmd);
static void n_getselectionenabled(void* slf, nCmd* cmd);
static void n_setselectionindex(void* slf, nCmd* cmd);
static void n_getselectionindex(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguitextview

    @cppclass
    nGuiTextView

    @superclass
    nguiwidget

    @classinfo
    Simple widget which renders a vertical list of text lines.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfont_s",               'SFNT', n_setfont);
    cl->AddCmd("s_getfont_v",               'GFNT', n_getfont);
    cl->AddCmd("v_setborder_s",             'SBRD', n_setborder);
    cl->AddCmd("s_getborder_v",             'GBRD', n_getborder);
    cl->AddCmd("v_settextcolor_ffff",       'STXC', n_settextcolor);
    cl->AddCmd("ffff_gettextcolor_v",       'GTXC', n_gettextcolor);
    cl->AddCmd("v_beginappend_v",           'BGAP', n_beginappend);
    cl->AddCmd("v_appendline_s",            'APPL', n_appendline);
    cl->AddCmd("v_endappend_v",             'EDAP', n_endappend);
    cl->AddCmd("i_getnumvisiblelines_v",    'GNVL', n_getnumvisiblelines);
    cl->AddCmd("v_setselectionenabled_b",   'SSLE', n_setselectionenabled);
    cl->AddCmd("i_getselectionenabled_v",   'GSLE', n_getselectionenabled);
    cl->AddCmd("v_setselectionindex_i",     'SSLI', n_setselectionindex);
    cl->AddCmd("i_getselectionindex_v",     'GSLI', n_getselectionindex);
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
    nGuiTextView* self = (nGuiTextView*) slf;
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
    nGuiTextView* self = (nGuiTextView*) slf;
    cmd->Out()->SetS(self->GetFont().Get());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setborder
    @input
    f(HoriBorder)
    @output
    v
    @info
    Set a horizontal border for the text.
*/
static void
n_setborder(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    self->SetBorder(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getborder
    @input
    v
    @output
    f(HoriBorder)
    @info
    Get the horizontal border for the text.
*/
static void
n_getborder(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    cmd->Out()->SetF(self->GetBorder());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settextcolor
    @input
    ffff(TextColor)
    @output
    v
    @info
    Set the text color.
*/
static void
n_settextcolor(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    static vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetTextColor(v);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettextcolor
    @input
    v
    @output
    ffff(TextColor)
    @info
    Get the text color.
*/
static void
n_gettextcolor(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    const vector4& v = self->GetTextColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    beginappend
    @input
    v
    @output
    v
    @info
    Begin appending text.
*/
static void
n_beginappend(void* slf, nCmd* /*cmd*/)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    self->BeginAppend();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    appendline
    @input
    s(TextLine)
    @output
    v
    @info
    Append a line of text to the internal text array.
*/
static void
n_appendline(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    self->AppendLine(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    endappend
    @input
    v
    @output
    v
    @info
    Finish appending text.
*/
static void
n_endappend(void* slf, nCmd* /*cmd*/)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    self->EndAppend();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getnumvisiblelines
    @input
    v
    @output
    i
    @info
    Returns the number of fully visible lines which would fit into the
    widget's area.
*/
static void
n_getnumvisiblelines(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    cmd->Out()->SetI(self->GetNumVisibleLines());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setselectionindex
    @input
    i
    @output
    v
    @info
    Set the current selection index.
*/
static void
n_setselectionindex(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    self->SetSelectionIndex(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getselectionindex
    @input
    v
    @output
    i
    @info
    Get the current selection index.
*/
static void
n_getselectionindex(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    cmd->Out()->SetI(self->GetSelectionIndex());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setselectionenabled
    @input
    b
    @output
    v
    @info
    Enable/disable selection handling.
*/
static void
n_setselectionenabled(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    self->SetSelectionEnabled(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getselectionenabled
    @input
    v
    @output
    b
    @info
    Get selection handling status.
*/
static void
n_getselectionenabled(void* slf, nCmd* cmd)
{
    nGuiTextView* self = (nGuiTextView*) slf;
    cmd->Out()->SetB(self->GetSelectionEnabled());
}




