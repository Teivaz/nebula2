//------------------------------------------------------------------------------
//  nguidragbox_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidragbox.h"

static void n_setdragthreshold(void* slf, nCmd* cmd);
static void n_getdragthreshold(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguidragbox
    @cppclass
    nGuiDragBox
    @superclass
    nguiwidget
    @classinfo
    A drag box widget.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setdragthreshold_f", 'SDTS', n_setdragthreshold);
    cl->AddCmd("f_getdragthreshold_v", 'GDTS', n_getdragthreshold);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setdragthreshold
    @input
    f(DragThreshold)
    @output
    v
    @info
    Set threshold in screen space units until a drag operation becomes valid.
*/
static void
n_setdragthreshold(void* slf, nCmd* cmd)
{
    nGuiDragBox* self = (nGuiDragBox*) slf;
    self->SetDragThreshold(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getdragthreshold
    @input
    v
    @output
    f(DragThreshold)
    @info
    Get threshold in screen space units.
*/
static void
n_getdragthreshold(void* slf, nCmd* cmd)
{
    nGuiDragBox* self = (nGuiDragBox*) slf;
    cmd->Out()->SetF(self->GetDragThreshold());
}

