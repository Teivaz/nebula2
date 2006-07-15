//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "kernel/npersistserver.h"

static void n_setassign(void* slf, nCmd* cmd);
static void n_getassign(void* slf, nCmd* cmd);
static void n_manglepath(void* slf, nCmd* cmd);
static void n_fileexists(void* slf, nCmd* cmd);
static void n_createfilenode(void* slf, nCmd* cmd);
static void n_copyfile(void* slf, nCmd* cmd);
static void n_deletefile(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfileserver2

    @cppclass
    nFileServer2

    @superclass
    nroot

    @classinfo
    New style fileserver.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setassign_ss",        'SASS', n_setassign);
    cl->AddCmd("s_getassign_s",         'GASS', n_getassign);
    cl->AddCmd("s_manglepath_s",        'MNGP', n_manglepath);
    cl->AddCmd("b_fileexists_s",        'FLEX', n_fileexists);
    cl->AddCmd("o_createfilenode_s",    'CFLN', n_createfilenode);
    cl->AddCmd("b_copyfile_ss",         'CPFL', n_copyfile);
    cl->AddCmd("b_deletefile_s",        'DLFL', n_deletefile);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setassign

    @input
    s(AssignName), s(Path)

    @output
    v

    @info
    Defines an assign with the specified name and links it to the specified
    path.
*/
static void
n_setassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetAssign(s0, s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getassign

    @input
    s(AssignName)

    @output
    s(Path)

    @info
    Get a path associated with an assign.
*/
static void
n_getassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetS(self->GetAssign(cmd->In()->GetS()).Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    manglepath

    @input
    s(UnmangledPath)

    @output
    s(MangledPath)

    @info
    Convert a path with assigns into a native absolute path.
*/
static void
n_manglepath(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    nString str = self->ManglePath(cmd->In()->GetS());
    cmd->Out()->SetS(str.Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    fileexists

    @input
    s(PathName)

    @output
    b(Exists)

    @info
    Return true if file exists. Note: does not work for directories!
*/
static void
n_fileexists(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetB(self->FileExists(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    createfilenode

    @input
    s(UniqueName)

    @output
    o(Filenode)

    @info
    Create a Nebula2 filenode and return it. A filenode
    wraps a nFile object into a nRoot node, and offers a script interface.
    Use a nfilenode object for file access from within MicroTcl, or other
    script functions which don't have their own file access functions.
*/
static void
n_createfilenode(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetO(self->CreateFileNode(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    copyfile
    @input
    s(FromName), s(ToName)
    @output
    b (Success)
    @info
    Copy a file.
*/
static void
n_copyfile(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    nString from = cmd->In()->GetS();
    nString to   = cmd->In()->GetS();
    cmd->Out()->SetB(self->CopyFile(from, to));
}

//------------------------------------------------------------------------------
/**
    @cmd
    deletefile
    @input
    s(FileName)
    @output
    b (Success)
    @info
    Delete a file.
*/
static void
n_deletefile(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetB(self->DeleteFile(cmd->In()->GetS()));
}
