//------------------------------------------------------------------------------
//  nguiflipbutton_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiflipbutton.h"

static void n_addstate(void* slf, nCmd* cmd);
static void n_setstate(void* slf, nCmd* cmd);
static void n_getstate(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiflipbutton

    @cppclass
    nGuiFlipButton

    @superclass
    nguitextlabel

    @classinfo
    A button which flips through different states.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addstate_s", 'ADDS', n_addstate);
    cl->AddCmd("v_setstate_i", 'SSTT', n_setstate);
    cl->AddCmd("i_getstate_v", 'GSTT', n_getstate);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addstate
    @input
    s(StateText)
    @output
    v
    @info
    Add a state to the button.
*/
static void
n_addstate(void* slf, nCmd* cmd)
{
    nGuiFlipButton* self = (nGuiFlipButton*) slf;
    self->AddState(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setstate
    @input
    i(StateIndex)
    @output
    v
    @info
    Set a state by its index.
*/
static void
n_setstate(void* slf, nCmd* cmd)
{
    nGuiFlipButton* self = (nGuiFlipButton*) slf;
    self->SetState(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getstate
    @input
    v
    @output
    i(StateIndex)
    @info
    Get the current state's index.
*/
static void
n_getstate(void* slf, nCmd* cmd)
{
    nGuiFlipButton* self = (nGuiFlipButton*) slf;
    cmd->Out()->SetI(self->GetState());
}


