//------------------------------------------------------------------------------
//  nguicheckbuttongroup_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicheckbuttongroup.h"

static void n_setonbuttonindex(void* slf, nCmd* cmd);
static void n_getonbuttonindex(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguicheckbuttongroup

    @cppclass
    nGuiCheckButtonGroup

    @superclass
    nguiwidget

    @classinfo
    A on/off check button.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setonbuttonindex_i", 'SBTI', n_setonbuttonindex);
    cl->AddCmd("i_getonbuttonindex_v", 'GBTI', n_getonbuttonindex);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setonbuttonindex
    @input
    i(OnButtonIndex)
    @output
    v
    @info
    Manually set the "on" button by index.
*/
static void
n_setonbuttonindex(void* slf, nCmd* cmd)
{
    nGuiCheckButtonGroup* self = (nGuiCheckButtonGroup*) slf;
    self->SetOnButtonIndex(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getonbuttonindex
    @input
    v
    @output
    i(OnButtonIndex)
    @info
    Manually get the "on" button by index.    
*/
static void
n_getonbuttonindex(void* slf, nCmd* cmd)
{
    nGuiCheckButtonGroup* self = (nGuiCheckButtonGroup*) slf;
    cmd->Out()->SetI(self->GetOnButtonIndex());
}
