//------------------------------------------------------------------------------
//  nguicheckbutton_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicheckbutton.h"

static void n_setstate(void* slf, nCmd* cmd);
static void n_getstate(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguicheckbutton

    @cppclass
    nGuiCheckButton

    @superclass
    nguitextlabel

    @classinfo
    A on/off check button.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setstate_b", 'STST', n_setstate);
    cl->AddCmd("b_getstate_v", 'GTST', n_getstate);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setstate
    @input
    b(ButtonState)
    @output
    v
    @info
    Manually set the button state.
*/
static void
n_setstate(void* slf, nCmd* cmd)
{
    nGuiCheckButton* self = (nGuiCheckButton*) slf;
    self->SetState(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getstate
    @input
    v
    @output
    b(ButtonState)
    @info
    Get the current button state.
*/
static void
n_getstate(void* slf, nCmd* cmd)
{
    nGuiCheckButton* self = (nGuiCheckButton*) slf;
    cmd->Out()->SetB(self->GetState());
}
