//------------------------------------------------------------------------------
//  nguilister_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguilister.h"

static void n_clear(void* slf, nCmd* cmd);
static void n_appendtext(void* slf, nCmd* cmd);
static void n_gettext(void* slf, nCmd* cmd);
static void n_setselectedindex(void* slf, nCmd* cmd);
static void n_getselectedindex(void* slf, nCmd* cmd);
static void n_getnumentries(void* slf, nCmd* cmd);
static void n_setcursorbrush(void* slf, nCmd* cmd);
static void n_getcursorbrush(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguilister
    @superclass
    nguiwidget
    @classinfo
    A lister widget.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_clear_v",                 'CLER', n_clear);
    cl->AddCmd("v_appendtext_s",            'ATXT', n_appendtext);
    cl->AddCmd("s_gettext_v",               'GTXT', n_gettext);
    cl->AddCmd("v_setselectedindex_i",      'SSLI', n_setselectedindex);
    cl->AddCmd("i_getselectedindex_v",      'GSLI', n_getselectedindex);
    cl->AddCmd("i_getnumentries_v",         'GNME', n_getnumentries);
    cl->AddCmd("v_setcursorbrush_s",        'SCRB', n_setcursorbrush);
    cl->AddCmd("s_getcursorbrush_v",        'GCRB', n_getcursorbrush);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    clear
    @input
    v
    @output
    v
    @info
    Clears all items.
*/
static void
n_clear(void* slf, nCmd* /*cmd*/)
{
    nGuiLister* self = (nGuiLister*) slf;
    self->Clear();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    appendtext
    @input
    s
    @output
    v
    @info
    Appends a string to the array of available items.
*/
static void
n_appendtext(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    self->AppendText(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettext
    @input
    v
    @output
    s
    @info
    Get selected text or text typed by the user.
*/
static void
n_gettext(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    cmd->Out()->SetS(self->GetText());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setselectedindex
    @input
    i
    @output
    v
    @info
    Set the index of the currently selected entry.
*/
static void
n_setselectedindex(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    self->SetSelectedIndex(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getselectedindex
    @input
    v
    @output
    i
    @info
    Get the index of the currently selected entry.
*/
static void
n_getselectedindex(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    cmd->Out()->SetI(self->GetSelectedIndex());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getnumentries
    @input
    v
    @output
    i(NumEntries)
    @info
    Get number of entries in lister.
*/
static void
n_getnumentries(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    cmd->Out()->SetI(self->GetNumEntries());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setcursorbrush
    @input
    s(CursorBrush)
    @output
    v
    @info
    Set the cursor brush.
*/
static void
n_setcursorbrush(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    self->SetCursorBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getcursorbrush
    @input
    v
    @output
    s(CursorBrush)
    @info
    Get the cursor brush name.
*/
static void
n_getcursorbrush(void* slf, nCmd* cmd)
{
    nGuiLister* self = (nGuiLister*) slf;
    cmd->Out()->SetS(self->GetCursorBrush());
}

