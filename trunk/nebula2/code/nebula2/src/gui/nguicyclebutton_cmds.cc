//------------------------------------------------------------------------------
//  nguicyclebutton_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicyclebutton.h"

static void n_puttextlabel(void* slf, nCmd* cmd);
static void n_hastextlabel(void* slf, nCmd* cmd);
static void n_removetextlabel(void* slf, nCmd* cmd);
static void n_removealltextlabels(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguicyclebutton

    @cppclass
    nGuiCycleButton

    @superclass
    nguitextlabel

    @classinfo
    A cycle button widget.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_puttextlabel_s",          'PTTL', n_puttextlabel);
    cl->AddCmd("b_hastextlabel_s",          'HSTL', n_hastextlabel);
    cl->AddCmd("v_removetextlabel_s",       'RMTL', n_removetextlabel);
    cl->AddCmd("v_removealltextlabels_v",   'RMAL', n_removealltextlabels);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    puttextlabel
    @input
    s
    @output
    v
    @info
    Extend circular list of text labels
*/
static void 
n_puttextlabel(void* slf, nCmd* cmd)
{
    nGuiCycleButton* self = (nGuiCycleButton*) slf;
    self->PutTextLabel(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    hastextlabel
    @input
    s
    @output
    b
    @info
    Is text label available?
*/
static void 
n_hastextlabel(void* slf, nCmd* cmd)
{
    nGuiCycleButton* self = (nGuiCycleButton*) slf;
    cmd->Out()->SetB(self->HasTextLabel(cmd->In()->GetS()));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    removetextlabel
    @input
    s
    @output
    v
    @info
    Remove text label from circular list. The label must exists.
*/
static void 
n_removetextlabel(void* slf, nCmd* cmd)
{
    nGuiCycleButton* self = (nGuiCycleButton*) slf;
    self->RemoveTextLabel(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    removealltextlabels
    @input
    v
    @output
    v
    @info
    Remove all textlabels from circular list.
*/
static void 
n_removealltextlabels(void* slf, nCmd* /*cmd*/)
{
    nGuiCycleButton* self = (nGuiCycleButton*) slf;
    self->RemoveAllTextLabels();
}
