//------------------------------------------------------------------------------
//  nguitextentry_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextentry.h"

static void n_setcursorbrush(void* slf, nCmd* cmd);
static void n_getcursorbrush(void* slf, nCmd* cmd);
static void n_setmaxlength(void* slf, nCmd* cmd);
static void n_getmaxlength(void* slf, nCmd* cmd);
static void n_setactive(void* slf, nCmd* cmd);
static void n_getactive(void* slf, nCmd* cmd);
static void n_setemptytext(void* slf, nCmd* cmd);
static void n_getemptytext(void* slf, nCmd* cmd);
static void n_setoverstrike(void* slf, nCmd* cmd);
static void n_setinitialcursorpos(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguitextentry

    @cppclass
    nGuiTextEntry

    @superclass
    nguitextlabel

    @classinfo
    A widget which is a text entry field.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcursorbrush_s",      'SCRB', n_setcursorbrush);
    cl->AddCmd("s_getcursorbrush_v",      'GCRB', n_getcursorbrush);
    cl->AddCmd("v_setmaxlength_i",        'SMXL', n_setmaxlength);
    cl->AddCmd("i_getmaxlength_v",        'GMXL', n_getmaxlength);
    cl->AddCmd("v_setactive_b",           'SACT', n_setactive);
    cl->AddCmd("b_getactive_v",           'GACT', n_getactive);
    cl->AddCmd("v_setemptytext_s",        'SEMT', n_setemptytext);
    cl->AddCmd("s_getemptytext_v",        'GEMT', n_getemptytext);
    cl->AddCmd("v_setoverstrike_b",       'SOST', n_setoverstrike);
    cl->AddCmd("v_setinitialcursorpos_s", 'SICP', n_setinitialcursorpos);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setcursorbrush
    @input
    s(CursorBrushName)
    @output
    v
    @info
    Set the name of the cursor brush.
*/
static void
n_setcursorbrush(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    self->SetCursorBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getcursorbrush
    @input
    v
    @output
    s(CursorBrushName)
    @info
    Get the name of the cursor brush.
*/
static void
n_getcursorbrush(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    cmd->Out()->SetS(self->GetCursorBrush());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setmaxlength
    @input
    i(MaxLineLength)
    @output
    v
    @info
    Set max number of chars in text line. Default is 32.
*/
static void
n_setmaxlength(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    self->SetMaxLength(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getmaxlength
    @input
    v
    @output
    i(MaxLineLength)
    @info
    Get max number of chars in text line. Default is 32.
*/
static void
n_getmaxlength(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    cmd->Out()->SetI(self->GetMaxLength());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setactive
    @input
    b(ActiveFlag)
    @output
    v
    @info
    Manually activate the text entry widget.
*/
static void
n_setactive(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    self->SetActive(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getactive
    @input
    v
    @output
    b(ActiveFlag)
    @info
    Get active state of widget.
*/
static void
n_getactive(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    cmd->Out()->SetB(self->GetActive());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setemptytext
    @input
    s(EmptyText)
    @output
    v
    @info
    Set optional empty replacement text.
*/
static void
n_setemptytext(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    self->SetEmptyText(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getemptytext
    @input
    v
    @output
    s(EmptyText)
    @info
    Get optional empty replacement text.
*/
static void
n_getemptytext(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    cmd->Out()->SetS(self->GetEmptyText());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setoverstrike
    @input
    b(do overstrike?)
    @output
    v
    @info
    Sets the initial state for the text entry field to overstrike or insert.
    Insert is the default.
*/
static void
n_setoverstrike(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    self->SetOverstrike(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setinitialcursorpos
    @input
    s(Alignment = left | right)
    @output
    v
    @info
    Whenever the field is activated, the cursor is reset to this position
    Left means "on the first character"
    Right means "after the last character"
*/
static void
n_setinitialcursorpos(void* slf, nCmd* cmd)
{
    nGuiTextEntry* self = (nGuiTextEntry*) slf;
    const char* str = cmd->In()->GetS();
    nGuiTextLabel::Alignment pos;
    if (strcmp("left", str) == 0)        pos = nGuiTextLabel::Left;
    else if (strcmp("right", str) == 0)  pos = nGuiTextLabel::Right;
    else
    {
        n_error("nguitextentry.setinitialcursorpos: Invalid string '%s'", str);
        return;
    }
    self->SetInitialCursorPos(pos);
}
