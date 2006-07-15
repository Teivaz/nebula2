//------------------------------------------------------------------------------
//  nguidirlister_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidirlister.h"

static void n_setdirectory(void* slf, nCmd* cmd);
static void n_getdirectory(void* slf, nCmd* cmd);
static void n_setignoresubdirs(void* slf, nCmd* cmd);
static void n_getignoresubdirs(void* slf, nCmd* cmd);
static void n_setignorefiles(void* slf, nCmd* cmd);
static void n_getignorefiles(void* slf, nCmd* cmd);
static void n_setpattern(void* slf, nCmd* cmd);
static void n_getpattern(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguidirlister

    @cppclass
    nGuiDirLister

    @superclass
    nguitextview

    @classinfo
    A directory content lister widget.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setdirectory_s",      'SDIR', n_setdirectory);
    cl->AddCmd("s_getdirectory_v",      'GDIR', n_getdirectory);
    cl->AddCmd("v_setignoresubdirs_b",  'SISD', n_setignoresubdirs);
    cl->AddCmd("b_getignoresubdirs_v",  'GISD', n_getignoresubdirs);
    cl->AddCmd("v_setignorefiles_b",    'SIFL', n_setignorefiles);
    cl->AddCmd("b_getignorefiles_v",    'GIFL', n_getignorefiles);
    cl->AddCmd("v_setpattern_s",        'SPTN', n_setpattern);
    cl->AddCmd("s_getpattern_v",        'GPTN', n_getpattern);
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
    Set the directory name.
*/
static void
n_setdirectory(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
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
    Set the directory name.
*/
static void
n_getdirectory(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    cmd->Out()->SetS(self->GetDirectory());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setignoresubdirs
    @input
    b(IgnoreSubDirs)
    @output
    v
    @info
    If true, subdirectories will not be listed.
*/
static void
n_setignoresubdirs(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    self->SetIgnoreSubDirs(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getignoresubdirs
    @input
    v
    @output
    b(IgnoreSubDirs)
    @info
    Return "ignore subdirs" flag.
*/
static void
n_getignoresubdirs(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    cmd->Out()->SetB(self->GetIgnoreSubDirs());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setignorefiles
    @input
    b(IgnoreFiles)
    @output
    v
    @info
    If true, files will not be listed.
*/
static void
n_setignorefiles(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    self->SetIgnoreFiles(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getignorefiles
    @input
    v
    @output
    b(IgnoreFiles)
    @info
    Return "ignore files" flag.
*/
static void
n_getignorefiles(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    cmd->Out()->SetB(self->GetIgnoreFiles());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setpattern
    @input
    s(Pattern)
    @output
    v
    @info
    Set file pattern against which entries will be matched.
*/
static void
n_setpattern(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    self->SetPattern(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getpattern
    @input
    v
    @output
    s(Pattern)
    @info
    Get the file pattern.
*/
static void
n_getpattern(void* slf, nCmd* cmd)
{
    nGuiDirLister* self = (nGuiDirLister*) slf;
    cmd->Out()->SetS(self->GetPattern());
}








