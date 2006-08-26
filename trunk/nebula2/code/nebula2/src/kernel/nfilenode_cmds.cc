//------------------------------------------------------------------------------
//  nfilenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfilenode.h"

static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_puts(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfilenode

    @cppclass
    nFileNode

    @superclass
    nroot

    @classinfo
    NOH representation of a file. This is useful for scripting languages without
    native file I/O support, like Nebula's MicroTCL.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_open_ss",  'OPEN', n_open);
    cl->AddCmd("v_close_v",  'CLOS', n_close);
    cl->AddCmd("b_isopen_v", 'ISOP', n_isopen);
    cl->AddCmd("b_puts_s",   'PUTS', n_puts);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    open

    @input
    s(Filename), s(AccessMode)

    @output
    v

    @info
    Open a file. Filename can be a Nebula filename using path assigns.
    AccessMode is a string containing an "r" for read-mode, and/or
    "w" for write-mode.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nFileNode* self = (nFileNode*) slf;
    nString s0 = cmd->In()->GetS();
    nString s1 = cmd->In()->GetS();
    cmd->Out()->SetB(self->Open(s0, s1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    close

    @input
    v

    @output
    v

    @info
    Closes a file.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nFileNode* self = (nFileNode*) slf;
    self->Close();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isopen

    @input
    v

    @output
    b(IsOpen)

    @info
    Return "true" if file is open.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nFileNode* self = (nFileNode*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    puts

    @input
    s(String)

    @output
    b(Success)

    @info
    Put a string into the file.
*/
static void
n_puts(void* slf, nCmd* cmd)
{
    nFileNode* self = (nFileNode*) slf;
    cmd->Out()->SetB(self->PutS(cmd->In()->GetS()));
}
