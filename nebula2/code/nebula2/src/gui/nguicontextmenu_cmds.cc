//------------------------------------------------------------------------------
//  nguicontextmenu_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicontextmenu.h"

static void n_addentry(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiwidget
    @cppclass
    nGuiContextMenu
    @superclass
    nroot
    @classinfo
    Ancestor of all gui widgets.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addentry_s", 'ADDE', n_addentry);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addentry
    @input
    s(Entry)
    @output
    v
    @info
    Add a new entry to the context menu
*/
static void
n_addentry(void* slf, nCmd* cmd)
{
    nGuiContextMenu* self = (nGuiContextMenu*) slf;
    self->AddEntry(cmd->In()->GetS());
}


