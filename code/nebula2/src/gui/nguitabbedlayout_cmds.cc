//------------------------------------------------------------------------------
//  nguitabbedlayout_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitabbedlayout.h"

static void n_setnumtabs(void* slf, nCmd* cmd);
static void n_getnumtabs(void* slf, nCmd* cmd);
static void n_setactivetab(void* slf, nCmd* cmd);
static void n_getactivetab(void* slf, nCmd* cmd);
static void n_gettabbuttonat(void* slf, nCmd* cmd);
static void n_getchildlayoutat(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguitabbedlayout

    @cppclass
    nGuiTabbedLayout

    @superclass
    nguiformlayout

    @classinfo
    A tabbed layout extends a form layout by a row of tab buttons, and
    a number of child form layouts which are switched visible by the
    tab buttons.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setnumtabs_i",        'SNTB', n_setnumtabs);
    cl->AddCmd("i_getnumtabs_v",        'GNTB', n_getnumtabs);
    cl->AddCmd("v_setactivetab_i",      'SATB', n_setactivetab);
    cl->AddCmd("i_getactivetab_v",      'GATB', n_getactivetab);
    cl->AddCmd("o_gettabbuttonat_i",    'GTBA', n_gettabbuttonat);
    cl->AddCmd("o_getchildlayoutat_i",  'GCHL', n_getchildlayoutat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setnumtabs
    @input
    i(NumTabs)
    @output
    v
    @info
    Set the number of tabs in the tab layout.
*/
static void
n_setnumtabs(void* slf, nCmd* cmd)
{
    nGuiTabbedLayout* self = (nGuiTabbedLayout*) slf;
    self->SetNumTabs(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumtabs
    @input
    v
    @output
    i(NumTabs)
    @info
    Returns number of tabs in the tab layout.
*/
static void
n_getnumtabs(void* slf, nCmd* cmd)
{
    nGuiTabbedLayout* self = (nGuiTabbedLayout*) slf;
    cmd->Out()->SetI(self->GetNumTabs());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setactivetab
    @input
    i(TabIndex)
    @output
    v
    @info
    Set the active tab. The child layout of this tab will be made visible.
*/
static void
n_setactivetab(void* slf, nCmd* cmd)
{
    nGuiTabbedLayout* self = (nGuiTabbedLayout*) slf;
    self->SetActiveTab(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getactivetab
    @input
    v
    @output
    i(TabIndex)
    @info
    Returns the index of the active tab.
*/
static void
n_getactivetab(void* slf, nCmd* cmd)
{
    nGuiTabbedLayout* self = (nGuiTabbedLayout*) slf;
    cmd->Out()->SetI(self->GetActiveTab());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettabbuttonat
    @input
    i(TabIndex)
    @output
    o(TabButtonObject)
    @info
    Returns the tab button object for the given tab index.
*/
static void
n_gettabbuttonat(void* slf, nCmd* cmd)
{
    nGuiTabbedLayout* self = (nGuiTabbedLayout*) slf;
    cmd->Out()->SetO(self->GetTabButtonAt(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchildlayoutat
    @input
    i(TabIndex)
    @output
    o(ChildLayoutObject)
    @info
    Returns the child layout object for the given tab index.
*/
static void
n_getchildlayoutat(void* slf, nCmd* cmd)
{
    nGuiTabbedLayout* self = (nGuiTabbedLayout*) slf;
    cmd->Out()->SetO(self->GetChildLayoutAt(cmd->In()->GetI()));
}

