//------------------------------------------------------------------------------
//  nguifilelister_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguifilelister.h"

static void n_setdirectory(void* slf, nCmd* cmd);
static void n_getdirectory(void* slf, nCmd* cmd);
static void n_getfilename(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguifilelister
    @superclass
    nguiwidget
    @classinfo
    A file lister widget.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setdirectory_s", 'SDIR', n_setdirectory);
    cl->AddCmd("s_getdirectory_v", 'GDIR', n_getdirectory);
    cl->AddCmd("s_getfilename_v",  'GFLN', n_getfilename);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setdirectory
    @input
    s(DirName)
    @output
    v
    @info
    Set the directory name to list as Nebula filesystem path.
*/
static void
n_setdirectory(void* slf, nCmd* cmd)
{
    nGuiFileLister* self = (nGuiFileLister*) slf;
    self->SetDirectory(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getdirectory
    @input
    v
    @output
    s(DirName)
    @info
    Get the directory name to list as Nebula filesystem path.
*/
static void
n_getdirectory(void* slf, nCmd* cmd)
{
    nGuiFileLister* self = (nGuiFileLister*) slf;
    cmd->Out()->SetS(self->GetDirectory());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getfilename
    @input
    v
    @output
    s(FileName)
    @info
    Get the filename selected by the user. This must not necessarly be
    an existing file!
*/
static void
n_getfilename(void* slf, nCmd* cmd)
{
    nGuiFileLister* self = (nGuiFileLister*) slf;
    cmd->Out()->SetS(self->GetFilename());
}
