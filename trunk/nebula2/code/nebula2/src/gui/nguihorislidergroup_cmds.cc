//------------------------------------------------------------------------------
//  nguihorislidergroup_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihorislidergroup.h"

static void n_setlefttext(void* slf, nCmd* cmd);
//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguihorislidergroup

    @cppclass
    nGuiHoriSliderGroup

    @superclass
    nguiformlayout

    @classinfo
    A horizontal slider group consisting of a label, a horizontal slider,
    and a text label which displays the current slider's numerical value
    as a printf-formatted string.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setlefttext_s",    'SLFT', n_setlefttext);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setlefttext
    @input
    s(text)
    @output
    v
    @info
    Set the name of the cursor brush.
*/
static void
n_setlefttext(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetLeftText(cmd->In()->GetS());
}
